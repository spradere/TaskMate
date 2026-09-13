# Audit général du build — sûreté des fichiers et répertoires

Date : 13 septembre 2026  
Branche auditée : `codex/build-audit`  
Révision auditée : `38a2280a8dd5ab7cb4f32d5fa6b814b7ca240174`  
Environnement : FreeBSD 16.0-CURRENT, BSD `bmake` 20260704

## Objet et position sur la portabilité

Cet audit examine principalement les créations, remplacements, suppressions et copies de fichiers,
la construction des chemins, les fichiers temporaires, les interruptions et les exécutions
concurrentes. Il couvre le Makefile principal, `mk/*.mk`, les fragments matériels sélectionnés, les
scripts `sh`/AWK et le sous-système de fichiers d'autoCode.

Le maintien volontaire de BSD `bmake` et de FreeBSD est considéré comme une contrainte de conception
valide. Il n'est pas recommandé d'ajouter GNU Make, Linux, CMake ou un autre orchestrateur à ce stade.
Au contraire, limiter la plateforme réduit la matrice de validation et permet de concentrer l'effort
sur le code système. Les extensions utilisées (`.WAIT`, `:T`, `:ts`, `find -delete`, `realpath`) doivent
simplement être assumées, documentées et testées sur cette plateforme.

## Modèle de menace

Le build s'exécute avec les droits du développeur et ses Makefiles sont du code exécutable. Il ne peut
donc pas constituer une frontière de sécurité contre une branche Git volontairement malveillante.
L'objectif réaliste est de protéger les fichiers du développeur contre :

- une variable vide, erronée ou surchargée ;
- un chemin inattendu ou un lien symbolique ;
- une interruption, un disque plein ou une erreur d'entrée/sortie ;
- deux builds lancés en parallèle ;
- un fichier de configuration mal formé ;
- une sauvegarde dirigée vers le mauvais système de fichiers.

Les constats de sécurité ci-dessous supposent un dépôt normalement digne de confiance, mais pas un
environnement parfaitement fiable.

## Verdict

La base est saine pour un usage local, mono-utilisateur et séquentiel : les suppressions sont désormais
précédées d'un confinement canonique sous `build/`, les écritures autoCode sont différées, les erreurs
de lecture sont bornées et les artefacts ordinaires sont séparés des sources.

Le build n'est toutefois pas encore robuste face aux erreurs de fichiers les plus importantes. Le
risque principal est la cible `backup`, qui emploie `rsync --delete` sans prouver que le point de montage
correspond exactement au périphérique attendu. Viennent ensuite un contrôle d'architecture dont le
code d'échec est masqué, des fichiers temporaires à nom prévisible, plusieurs écritures non atomiques et
l'absence de contrat contre les builds concurrents.

| Priorité | Constat | Conséquence principale |
| --- | --- | --- |
| Critique | `backup` ne valide pas exactement le montage avant `rsync --delete` | Suppression possible dans un répertoire du disque local ou sur le mauvais volume |
| Haute | Le statut du contrôle d'architecture est consommé par la recette | Un build peut continuer malgré une erreur de validation des sources |
| Haute | Les fichiers `.tmp` sont prévisibles et parfois publiés sans arrêt immédiat sur erreur | Écrasement, course, lien symbolique ou publication partielle |
| Haute | Les listes autoCode et `.gitignore` sont écrites directement | Une interruption peut laisser un fichier partiel considéré comme à jour |
| Moyenne | autoCode remplace plusieurs destinations sans transaction globale | Une erreur tardive laisse un ensemble généré mixte |
| Moyenne | Le graphe n'interdit pas les builds parallèles ou simultanés | Courses sur les sources générées, journaux, stamps et résultats |
| Moyenne | La découverte et beaucoup de commandes utilisent des listes shell non citées | Les espaces, retours ligne et noms commençant par `-` ne sont pas supportés |
| Moyenne | `.BEGIN` et `.END` écrivent pour toutes les cibles | `help`, `clean` ou un échec modifient des métadonnées de build |
| Faible | Le nettoyage des anciens logs autoCode utilise un motif `find -name` incorrect | Accumulation non bornée des journaux |
| Faible | Quelques contrôles construisent une commande shell depuis leur configuration | Robustesse réduite aux caractères spéciaux et diagnostics indirects |

## Points solides

### Confinement des suppressions

`scripts/check_build_delete_path.sh` résout sa propre position, en déduit la racine canonique
`build/`, refuse les valeurs vides et rejette toute cible extérieure. La racine `build/` elle-même n'est
acceptée qu'avec `--allow-build-root`. Le script utilise `set -eu` et les appels de `clean`, `clean_hard`
et `autoCode_alone` lui passent les chemins entre guillemets (`scripts/check_build_delete_path.sh:13-68`,
`mk/utils.mk:15-79`, `mk/autoCode.mk:153-162`).

Les commandes destructives emploient aussi des limites utiles : `-type f`, `-maxdepth 1` ou
`-mindepth 1` selon le contrat. Le risque critique signalé par l'audit précédent de `find -delete` — une
surcharge directe vers `/` ou un chemin extérieur — est donc corrigé dans la révision courante.

La garde accepte les descendants attendus et refuse, lors de la validation ponctuelle, `build/` sans
l'option explicite ainsi que `/tmp`. Elle réduit très fortement les accidents de variable. Elle n'est
cependant pas une primitive atomique : un autre processus peut encore modifier un composant du chemin
entre la validation et le `find`.

### Traitement autoCode avant publication

autoCode lit les fichiers ligne par ligne avec une taille bornée et distingue fin de fichier,
troncature et erreur (`srcs/autoCode/fileUtility.c:167-206`). Il génère d'abord un fichier temporaire,
ferme les flux, vérifie les tags requis, puis ne lance la comparaison et le remplacement qu'après les
phases d'analyse (`srcs/autoCode/autoCode.c:135-165`, `srcs/autoCode/parseTag.c:133-258`).

Le remplacement d'une destination modifiée utilise directement `rename(temporaire, destination)` :
une erreur de `rename` ne supprime donc pas préalablement l'original. Les temporaires enregistrés sont
nettoyés après traitement et via `atexit()` (`srcs/autoCode/fileUtility.c:65-164,305-385`). Les erreurs
de flux et de `fclose()` sont détectées. Le corpus de test vérifie notamment qu'une erreur d'analyse ne
modifie pas une destination antérieure et qu'aucun `.tmp` ne subsiste
(`test/autoCode/autoCode_test.sh:91-117,433-475`).

### Périmètre et dépendances explicites

Les chemins principaux sont centralisés dans `mk/path_files.mk`, les fragments cible → carte → MCU →
architecture sont inclus explicitement, et les valeurs de cible absentes sont refusées à l'analyse du
Makefile. La liste des programmes requis est contrôlée par un script qui lit littéralement chaque ligne
et cite son argument (`scripts/check_programs.sh:17-44`). Le build ne télécharge ni dépendance ni code
pendant son exécution.

## Constats détaillés

### 1. Critique — la sauvegarde peut supprimer sur le mauvais système de fichiers

La cible `backup` décide que la clé est montée avec :

```sh
mount | grep -q "${PATH_USBKEY}"
```

Ce test cherche une sous-chaîne dans toute la sortie de `mount`. Il ne vérifie ni l'égalité du point de
montage, ni le périphérique `${FILE_USBDEV}`, ni le type `msdosfs`. Un autre montage dont la ligne
contient `/media/usbkey` peut donc produire un faux positif. La recette crée ensuite le répertoire et
exécute `rsync --delete --delete-excluded` (`mk/backup.mk:39-70`). Si la clé n'est pas réellement montée,
le répertoire `/media/usbkey/...` peut appartenir au système de fichiers local et son contenu être
supprimé pour refléter le dépôt.

La cible démonte aussi toujours `${PATH_USBKEY}`, même si le volume était déjà monté avant son lancement.
À l'inverse, un échec de `rsync` arrête la recette avant `umount`, faute de gestionnaire de sortie. Enfin,
le nom « backup » masque une sémantique de miroir : relancer la cible sur la même version supprime du
miroir les fichiers supprimés de la source ; il ne s'agit pas d'un instantané immuable.

Correction minimale recommandée :

1. lire la table de montage FreeBSD dans un format analysable et exiger l'égalité exacte du périphérique,
   du point de montage et du type ;
2. mémoriser si la recette a elle-même monté le volume ;
3. installer un `trap` qui ne démonte que dans ce cas ;
4. canonicaliser la destination et prouver qu'elle est un descendant strict du point de montage ;
5. exiger une sentinelle propre à la clé, puis lancer d'abord un `rsync --dry-run` affiché à l'opérateur ;
6. documenter explicitement le choix « miroir destructif » ou retirer `--delete` si un historique est
   attendu.

Cette correction peut rester entièrement FreeBSD/POSIX et ne justifie aucun changement de build system.

### 2. Haute — le contrôle d'architecture échoue, mais la recette réussit

`scripts/arch_include.awk` retourne `3` lorsqu'il détecte une violation. La recette
`_architecture_include_check` place cependant l'appel dans un `if`; sa branche `else` mémorise le statut,
affiche le journal et termine sur un `echo`, sans `exit "$status"` (`mk/header_allow.mk:51-62`). Le statut
final de la recette est donc zéro.

L'exécution directe du vérificateur sur la révision auditée retourne effectivement `3` et signale
13 violations. Le build peut néanmoins poursuivre vers les dépendances et la compilation. Cette erreur
de propagation contredit le contrat général selon lequel une validation de fichiers en échec doit
bloquer la publication du résultat.

Il faut afficher le journal puis rendre exactement le statut AWK. Le message `satus` doit aussi être
corrigé. Ce point est indépendant de la décision architecturale concernant les 13 violations : elles ne
doivent ni être masquées ni transformées en exceptions par cette correction.

### 3. Haute — noms temporaires prévisibles et ouverture non exclusive

Trois mécanismes utilisent un suffixe fixe `.tmp` :

- autoCode construit `<destination>.tmp` puis l'ouvre avec `fopen(..., "w+")`
  (`srcs/autoCode/fileUtility.c:289-303,375-385`) ;
- `scripts/compare_replace.sh` écrit `${destination}.tmp` (`scripts/compare_replace.sh:17-23`) ;
- `.BEGIN` écrit `srcs/interfaces/tm_info.h.tmp` (`mk/build.mk:20-39`).

Ces ouvertures tronquent un fichier existant et suivent un lien symbolique. Deux builds simultanés
écrivent le même temporaire. Un arrêt laisse parfois un `.tmp` dont le prochain lancement peut hériter.
autoCode enregistre et nettoie mieux ses temporaires, mais n'utilise ni création exclusive ni nom unique,
et le remplacement change le mode de la destination selon l'`umask` ayant créé le temporaire.

`compare_replace.sh` est plus fragile : il n'a ni contrôle d'arguments, ni `set -e`, ni `trap`. Si son
`printf` échoue, par exemple faute d'espace, le script continue vers `cmp` puis peut renommer un fichier
temporaire partiel ou ancien sur le fichier valide.

Il faut créer les temporaires dans le même répertoire que leur destination avec `mktemp` côté shell et
`mkstemp(3)` côté C, conserver leur nom dans un `trap`/registre, vérifier toutes les fermetures, puis
publier par `rename`. autoCode devrait contrôler le type de la destination avec `lstat`/`fstat`, définir
une politique explicite pour les liens symboliques et préserver le mode attendu.

### 4. Haute — plusieurs fichiers de contrôle sont tronqués en place

Les listes `files_error`, `files_initrc`, `files_to_parse`, `files_halinit`, `values_funcinit` et
`files_haldefine` sont vidées puis remplies ligne par ligne directement à leur emplacement final
(`mk/autoCode.mk:116-150`). Une interruption laisse une liste partielle avec une date récente. Au
lancement suivant, `bmake` peut la considérer à jour et autoCode peut travailler sur une composition
incomplète.

La génération de `.gitignore` suit le même modèle (`mk/backup.mk:15-37`). Une interruption peut modifier
temporairement la visibilité de nombreux artefacts et rendre une commande Git trop large plus risquée.
`build/last_build_info.txt`, les données CLOC et plusieurs journaux sont également écrits en place ; leur
impact est moindre, mais le diagnostic peut devenir incohérent.

Chaque générateur doit écrire un temporaire adjacent, vérifier le succès complet, puis effectuer un
`mv`/`rename`. Les listes doivent aussi être triées lorsqu'un ordre sémantique n'est pas requis. Pour les
`*.rc`, l'ordre ayant un effet sur la composition, il faut soit le définir explicitement, soit le
documenter comme contrat — pas dépendre de l'énumération de `find`.

### 5. Moyenne — la publication autoCode n'est atomique que fichier par fichier

Toutes les entrées sont analysées avant publication, ce qui protège bien des erreurs de syntaxe. En
revanche, `fileCmpReplaceAll()` remplace les destinations l'une après l'autre. Si le troisième `rename`
échoue, les deux premières destinations restent nouvelles et les suivantes anciennes
(`srcs/autoCode/fileUtility.c:65-101`). Le build échoue, mais l'arbre de sources est partiellement mis à
jour. La note d'architecture reconnaît déjà cette absence de rollback.

Deux stratégies sont acceptables :

- assumer explicitement « atomique par fichier, build bloqué en cas d'ensemble mixte », puis garantir
  qu'un prochain autoCode répare toujours l'ensemble ;
- ajouter un petit journal de transaction et des sauvegardes adjacentes permettant un rollback.

La première option est probablement proportionnée au projet embarqué actuel, à condition d'ajouter un
test injectant un échec de `rename` après au moins un succès.

### 6. Moyenne — le build parallèle n'a pas de contrat sûr

Le graphe emploie `.WAIT`, mais aucun `.NOTPARALLEL`, verrou par cible ou répertoire de session n'est
déclaré. Avec `bmake -j`, `_system_critical_check` et `_autocode` appartiennent au même groupe précédant
le premier `.WAIT` (`mk/build.mk:81-83`) : le contrôle peut lire les sources pendant qu'autoCode les
remplace. Après le second `.WAIT`, le lien, la mesure mémoire et CLOC sont aussi des prérequis frères ;
`_mcu_memory_data` ne dépend pas explicitement du binaire lié (`srcs/hal/arch/avr8/avr8_CC.mk:59-66`).

Deux invocations séquentielles mais simultanées sur la même cible partagent en outre les `.tmp`, stamps,
logs et fichiers générés dans les sources. Des cibles différentes partagent encore `build/autoCode`,
`build/log` et les mêmes destinations générées.

À court terme, déclarer le build non parallèle et refuser deux instances via un verrou FreeBSD est plus
simple que rendre toute la chaîne réentrante. `.WAIT` doit rester utilisé pour exprimer les dépendances
réelles, notamment binaire lié → mesure mémoire. Une parallélisation sélective des seules compilations
pourra être réintroduite plus tard si elle apporte un gain mesuré.

### 7. Moyenne — le domaine des noms de fichiers n'est pas défini

La découverte utilise `find` puis stocke sa sortie dans des listes Make séparées par espaces
(`mk/sources.mk:15-50`). De nombreuses recettes développent ensuite ces listes sans guillemets. Les noms
contenant espace, tabulation, retour ligne, glob ou commençant par `-` ne sont donc pas fiables. Les
écritures `.for` d'autoCode ont le même contrat implicite.

Essayer de supporter tous les noms Unix dans les listes texte de `bmake` ajouterait une complexité peu
utile ici. La solution adaptée est de formaliser et contrôler un alphabet de chemin simple, par exemple
lettres ASCII, chiffres, `_`, `-`, `.`, `/`, sans composant `.` ou `..`, puis de faire échouer le build
avant toute écriture si un fichier suivi ou découvert sort de ce domaine. Les appels directs sur un seul
chemin doivent néanmoins rester cités et employer `--` lorsque l'outil le permet.

### 8. Moyenne — `.BEGIN` et `.END` ont des effets de bord globaux

Le développement de `bmake -n help` montre que la cible d'aide vérifie les programmes, crée les
répertoires de build, prépare `tm_info.h.tmp` et réécrit `last_build_info.txt`. Ces opérations viennent
de `.BEGIN` et `.END` (`mk/build.mk:20-78`) et ne sont donc pas limitées à `all`/`upload`.

En particulier, `.END` décrit le « dernier build » même après une cible utilitaire ; selon la classe
d'échec rencontrée, ces métadonnées peuvent être tronquées ou décrire une tentative incomplète. La
génération de `tm_info.h` dans les sources est nécessaire au firmware, mais elle ne devrait pas être un
effet de bord de `help`, `clean`, `doc` ou des tests hôte.

Il faut déplacer ces recettes vers des cibles internes prérequises uniquement par les cibles qui en ont
besoin, et publier `last_build_info.txt` atomiquement seulement après le succès du build concerné.

### 9. Faible — le nettoyage des logs ne correspond jamais au nom de base

La recette appelle :

```make
find "${PATH_LOGS}" -maxdepth 1 -type f -name "${PATH_LOGS}/autoCode_log*" -delete
```

Or `find -name` compare uniquement le nom de base. Le motif contient `build/log/` et ne peut donc pas
correspondre (`mk/autoCode.mk:60-64`). Les logs datés, qui contiennent par ailleurs `:` — valide et naturel
sous FreeBSD — s'accumulent malgré `OPT_CLEAN_AUTOCODE_LOGS=yes`.

Utiliser `${FILE_AUTOCODE_LOG:T}*` ou le littéral `autoCode_log*` suffit. La garde canonique existante sur
`${PATH_LOGS}` doit être conservée.

### 10. Faible — le contrôle d'en-têtes construit une commande shell

`header_allow.awk` concatène `source_file` et `PATH_SOURCES` dans une chaîne passée à un pipe shell vers
`grep` (`scripts/header_allow.awk:143-168`). Le format autorise plus de caractères que ce que cette
construction sait citer. Dans un dépôt de confiance, le risque d'injection n'est pas une frontière de
sécurité réelle, mais un guillemet ou un métacaractère accidentel peut modifier le scan.

Le contrôle cherche aussi une chaîne dans tout le contenu, et non une directive `#include` lexicale ; il
peut donc inclure commentaires et chaînes. Il serait plus robuste de laisser AWK parcourir directement
la liste des sources, comme le vérificateur d'architecture, sans construire de commande shell.

## Observations secondaires

- La garde de suppression est une très bonne amélioration, mais son chemin racine est figé à
  `<racine du script>/build`. C'est cohérent avec le projet actuel ; toute future option de build hors
  arbre devra introduire une racine explicitement validée, pas contourner la garde.
- `clean_hard` vide la cible courante et les fichiers directement sous `build/`, mais conserve les
  répertoires des autres cibles. Son aide « Remove all target build files » est donc ambiguë.
- Le stamp de contrôle des programmes évite un coût répété, mais ne détecte pas un changement de `PATH`
  ou de version d'outil. Ce n'est pas un risque de suppression ; il faut seulement supprimer/invalider le
  stamp lorsqu'un diagnostic d'environnement est nécessaire.
- Les chemins absolus historiques de `cppcheck` et `tidy_autoCode` ne correspondent pas au checkout
  actuel. Cela nuit à la reproductibilité, sans créer directement de risque destructif.
- Le build injecte version Git, compteur et date dans ses sorties. La provenance est utile, mais le
  résultat n'est volontairement pas reproductible bit à bit.

## Plan de durcissement recommandé

### Étape 0 — corrections courtes et urgentes

1. Rendre le statut non nul du contrôle d'architecture à `bmake`.
2. Corriger le motif de suppression des logs.
3. Durcir `backup` avant sa prochaine utilisation : montage exact, sentinelle, destination canonique,
   gestion du démontage et décision explicite sur `--delete`.
4. Ajouter `set -eu`, validation d'arguments et temporaire unique à `compare_replace.sh`.

### Étape 1 — publication sûre des fichiers

1. Introduire un helper shell unique « écrire → fermer → comparer → renommer » avec temporaire adjacent.
2. L'utiliser pour les listes autoCode, `.gitignore`, `tm_info.h` et les fichiers de métadonnées.
3. Remplacer `<destination>.tmp` par `mkstemp(3)` dans autoCode, contrôler les types et préserver le mode.
4. Définir puis tester la politique sur les liens symboliques.

### Étape 2 — déterminisme et concurrence

1. Déclarer officiellement que le build complet est non parallèle et ajouter un verrou par checkout.
2. Corriger les dépendances du graphe avant toute réactivation de `-j`.
3. Fixer l'ordre des découvertes ou déclarer explicitement les listes dont l'ordre est sémantique.
4. Ajouter un validateur léger des noms de chemins supportés.

### Étape 3 — validation des chemins d'échec

Ajouter des tests isolés sous un répertoire temporaire pour : disque plein simulé, refus d'ouverture,
échec de fermeture, temporaire préexistant, lien symbolique, interruption, deux processus concurrents,
échec du deuxième `rename`, chemin avec espaces, montage absent et destination de sauvegarde incorrecte.
Les tests de sauvegarde doivent employer une arborescence ou un système de fichiers de test sans jamais
monter ni supprimer les données réelles de l'opérateur.

## Validation effectuée pendant l'audit

- lecture des documents d'architecture `build` et `autoCode`, des règles de style/interfaces et des
  Makefiles/scripts applicables ;
- inspection des valeurs développées par `bmake -V` pour la cible par défaut ;
- développement non exécuté de `bmake -n help`, confirmant les effets globaux de `.BEGIN`/`.END` ;
- exécution directe en lecture seule du vérificateur d'architecture : statut `3`, 13 violations ;
- validation syntaxique par `sh -n` de tous les scripts shell du périmètre ;
- validation ponctuelle de la garde : descendants de `build/` acceptés, `build/` et `/tmp` refusés ;
- vérification de l'absence actuelle de liens symboliques sous `build/` et de résidus `.tmp` dans le
  dépôt hors `.git`.

Aucun `clean`, `clean_hard`, `backup`, autoCode, build AVR ou test générateur n'a été exécuté : ces
commandes auraient modifié des artefacts ou des sources générées et n'étaient pas nécessaires pour
établir les constats. Aucune validation sur matériel physique n'est concernée par cet audit.

## Conclusion

Rester sur `bmake` et FreeBSD est raisonnable et même favorable à court terme. La priorité n'est pas la
portabilité du build, mais la réduction du nombre de chemins d'écriture et la transformation de chacun
en opération confinée, vérifiée et publiable atomiquement.

Le projet a déjà franchi l'étape la plus dangereuse pour `clean` grâce à la garde canonique. Le prochain
gain de sûreté vient d'un durcissement ciblé de `backup`, de la propagation stricte des statuts d'erreur,
de temporaires uniques et d'un contrat séquentiel explicite. Ces changements restent petits, locaux au
build hôte et n'ajoutent aucune charge de portage au code RTOS embarqué.
