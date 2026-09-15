# Audit d'extraction des zones générées par `autoCode`

Date : 15 septembre 2026

Branche auditée : `test`

Révision de base : `8c69b6f`

## Objet, périmètre et verdict

Cet audit étudie le retrait du code généré des onze fichiers `.c` et `.h` actuellement réécrits
par `autoCode`. La cible demandée conserve les tags dans les sources suivies par Git, mais leur
contenu devient un unique `#include` vers un fragment placé sous `./generated/`. Ces fragments ne
sont pas suivis par Git. L'audit couvre le générateur, BSD `bmake`, les dépendances du compilateur,
les contrôles d'architecture, les tests et les outils ; il ne modifie pas le firmware ni les zones
générées existantes.

La migration est **faisable et recommandée sous conditions**. Un fichier inclus est remplacé
textuellement par le préprocesseur : les définitions, déclarations et instructions actuelles
conservent donc leur portée, leurs attributs et les options de compilation de leur fichier
propriétaire. Aucun coût flash, RAM ou temps d'exécution n'est attendu.

Ce n'est toutefois pas un simple déplacement de texte. Quatre garanties doivent être intégrées au
même changement :

1. utiliser des fragments `.inc`, jamais des unités `.c` ni des en-têtes publics `.h` ;
2. isoler les sorties par `VAL_HW_STACK` pour empêcher le mélange de deux cibles ;
3. régénérer avant les contrôles et la compilation, y compris si un fragment a disparu ;
4. faire analyser les fragments avec la couche de leur fichier propriétaire.

Sans ces garanties, la proposition peut produire un firmware d'une autre cible ou masquer une
dépendance interdite tout en terminant la compilation avec succès.

## État actuel vérifié

`FILES_PARSE_TAG` contient onze destinations suivies par Git. `parseTag()` recopie chacune dans un
fichier temporaire, remplace le contenu compris entre les tags et diffère les renommages jusqu'à la
validation de tous les inputs. Douze tags sont requis, car `sys_modules.c` en possède deux.

Les zones représentent actuellement 318 lignes, commentaires et barrières `clang-format` compris.
Elles couvrent quatre contextes C distincts :

| Contexte d'inclusion | Tags | Nature du fragment |
| --- | --- | --- |
| portée d'en-tête | `error_enum`, `modules_count`, `gpio_signals` | types et macros |
| portée d'en-tête | `modules_list`, `hal_init`, `hal_define` | inclusions sélectionnées |
| portée de fichier `.c` | catalogues d'erreurs, threads et pilotes | objets et chaînes statiques |
| corps de fonction | `threads_alloc`, `drivers_alloc`, `hal_fxinit` | déclarations, instructions |

Le build normal exécute `_autocode` avant le contrôle d'architecture, les dépendances et la
compilation. AVR-GCC utilise déjà `-MMD -MP`, de sorte qu'un fragment inclus apparaîtra dans le
fichier `.d` de l'unité propriétaire. `-Isrcs -I./` permettrait un chemin racine, mais ne résout pas
à lui seul l'isolation entre cibles.

Le corpus courant passe 57 cas répartis par étape, puis 57 cas avec ASan/UBSan. Le contrôle
d'architecture courant passe avec 285 inclusions directes dans 94 fichiers. Ces résultats forment
une référence, mais les tests de remplacement supposent encore que le fichier tagué est la
destination et devront être réécrits.

## Forme cible recommandée

L'extension `.inc` exprime qu'un fichier est un fragment C dépendant de son contexte. Une extension
`.c` le ferait découvrir et compiler comme une unité autonome par `mk/sources.mk`. Une extension
`.h` suggérerait à tort une interface réutilisable, des gardes et une portée d'en-tête, alors que
trois fragments sont inclus au milieu d'une fonction.

Le tag suivi par Git doit rester lisible et entièrement vérifiable :

```c
// [autoCode_tag] threads_alloc
#include "autocode/system/sysCore/threads_alloc.inc"
// [/tag]
```

Pour la cible courante, le fichier correspondant serait par exemple :

```text
generated/test1_arduinoMega_atmega2560_avr8/
└── autocode/
    └── system/sysCore/threads_alloc.inc
```

Le build ajoute uniquement `generated/${VAL_HW_STACK:ts_}` au chemin d'inclusion. Le texte du
`#include` reste donc stable dans Git tandis que chaque composition matérielle possède ses propres
fragments. Le chemin après `autocode/` reflète la couche et le propriétaire logique ; il permet aux
contrôles d'attribuer le contenu inclus à la bonne source.

Les douze sorties doivent être distinctes. Elles ne portent pas de garde d'inclusion : leur portée
est imposée par le point d'inclusion et l'unicité de chaque tag. Chaque fragment peut commencer par
un court commentaire indiquant son tag, sa cible et qu'il ne doit pas être édité.

## Conditions de sûreté

### P0 — Isoler les cibles et éliminer les sorties périmées

Le stamp actuel est placé sous `build/${VAL_HW_STACK:ts_}`, alors que les destinations modifiées
sont globales dans `srcs/`. Après extraction, une sortie globale sous `generated/` créerait un cas
dangereux :

```text
build test1 -> fragments test1
build test_noscli -> mêmes fragments remplacés
retour test1 -> stamp test1 encore valide, fragments test_noscli utilisés
```

Le répertoire par pile matérielle supprime ce partage. La stratégie la plus simple et la plus sûre
est en plus d'exécuter le générateur à chaque build normal. La comparaison différée conserve le
mtime d'un fragment identique, donc les objets AVR ne sont pas recompilés inutilement. Cette
stratégie couvre aussi un checkout de branche avec des stamps récents mais des inputs plus anciens.

Si le stamp est conservé pour éviter l'exécution, un manifeste d'inputs et d'outputs avec empreinte
de contenu devient obligatoire. Une vérification préalable doit invalider le stamp lorsqu'un seul
des douze fragments manque. Les seules dépendances par mtime ne suffisent pas à cette garantie.

### P0 — Publier un fragment absent sans toucher aux sources

`fileCmpReplaceAll()` ouvre aujourd'hui l'ancienne destination en lecture seule avant de comparer.
Il échoue donc si une sortie n'existe pas, ce qui est précisément le cas d'un clone propre.
L'utilitaire doit traiter l'absence comme une différence valide et renommer le temporaire vers la
nouvelle destination.

`parseTag()` ne doit plus créer un temporaire à côté du `.c` ou `.h`. Il doit :

1. lire et valider les ancres suivies sans les ouvrir en écriture ;
2. associer chaque tag à un chemin `.inc` interne et borné ;
3. écrire chaque fragment dans le répertoire temporaire de la cible ;
4. fermer et valider toutes les sorties avant `fileCmpReplaceAll()` ;
5. laisser les onze sources suivies strictement inchangées.

La publication différée et le nettoyage `atexit()` doivent être conservés. La faiblesse actuelle
reste présente : une erreur de `rename()` après un premier remplacement peut publier un ensemble
partiel. Elle n'empêche pas la migration, mais un manifeste publié en dernier doit permettre de
détecter cet état et de forcer une régénération complète au build suivant.

### P0 — Ne pas créer une voie de contournement architectural

`arch_include.awk` ne scanne actuellement que `FILES_SRC` et `FILES_SRC_H`. Après extraction, il
verrait l'inclusion neutre `autocode/...inc`, mais plus les inclusions générées vers services,
tâches, interfaces, cible, carte, MCU et architecture. Le résultat serait un faux contrôle positif.

Le manifeste généré doit être consommé après `_autocode`. Pour chaque `.inc`, le contrôleur doit
retirer le préfixe de cible et `autocode/`, puis classifier le reste comme le chemin du
propriétaire.
Ainsi un include présent dans `autocode/system/sysCore/modules_list.inc` reste une relation émise
par `sysCore`.

Le contrôle des en-têtes critiques repose aussi sur une recherche limitée à `srcs/`. Il doit scanner
les fragments et attribuer toute référence à leur propriétaire suivi. Aucun fragment ne doit être
autorisé globalement sous prétexte qu'il est généré.

### P1 — Garantir l'ordre du build et les dépendances

Le répertoire et ses sous-répertoires doivent être créés par BSD `bmake` avant le lancement du
générateur. Le build conserve ensuite l'ordre strict :

```text
validation cible -> compilation autoCode -> génération -> contrôles -> dépendances -> AVR
```

Le chemin `generated/${VAL_HW_STACK:ts_}` doit être ajouté aux options du compilateur AVR, de
`cppcheck`, des outils d'éditeur et de toute commande qui prétraite les sources. Les `.inc` ne
doivent pas entrer dans `FILES_SRC`, `FILES_OBJ` ni être formatés comme du code manuel.

Les fichiers `.d` assurent les reconstructions incrémentales après le premier build. Le build doit
néanmoins dépendre explicitement du succès de la génération, car aucun `.d` n'existe encore sur un
clone propre.

### P1 — Encadrer Git, le nettoyage et les livrables

La `.gitignore` générée ignore déjà `generated/` grâce à sa règle globale, mais l'intention doit
être rendue explicite dans `mk/path_files.mk`, puis la `.gitignore` doit être régénérée. Elle ne
doit pas être éditée directement. Les tests doivent vérifier `git check-ignore` et l'absence de
fragment dans `git ls-files`.

Comme `generated/` se trouve hors de `build/`, les gardes actuels de suppression refuseront à juste
titre de l'effacer. `clean` ne doit recevoir aucune exception large. Il faut ajouter une validation
canonique dédiée qui n'autorise que le sous-répertoire de la pile courante sous la racine exacte
`generated/`, avec tests de sentinelles hors périmètre. `clean_hard` peut traiter la racine complète
seulement après une validation distincte explicite.

Un clone Git ne sera plus directement compilable sans le host tool. C'est acceptable si `bmake`
reste l'entrée obligatoire et compile `autoCode` avant AVR. Une archive de sources publiée doit
soit embarquer le générateur et ses inputs, soit inclure séparément des fragments pré-générés ; ces
derniers restent des artefacts de livraison, pas des fichiers suivis dans le dépôt.

### P2 — Préserver le diagnostic et la navigation

Les erreurs du compilateur nommeront le `.inc` au lieu du `.c` propriétaire. Le commentaire de
provenance et le chemin miroir rendent ce diagnostic exploitable. `__FILE__` dans un futur code
généré désignerait aussi le fragment ; ce comportement doit être documenté ou interdit dans les
fonctions d'émission.

Les recherches, ctags et IDE ne verront les symboles générés qu'après un premier `bmake`. Les cibles
d'éditeur peuvent indexer le manifeste de la pile sélectionnée, sans ajouter les fragments à la
liste des sources compilées. `cloc` et Doxygen doivent exclure explicitement la racine générée pour
ne pas compter ou publier des artefacts variables.

## Marche à suivre proposée

### Étape 0 — Fixer une référence reproductible

1. Conserver les logs des tests autoCode normaux et ASan/UBSan.
2. Construire proprement `test1` et `test_noscli` et conserver ELF, map, `avr-size` et symboles.
3. Relever les douze contenus générés et leur association exacte à la source propriétaire.
4. Vérifier le boot et les fonctions dépendantes sur Arduino Mega avant la migration.

### Étape 1 — Définir le contrat des fragments

1. Ajouter `PATH_GENERATED` et `PATH_GENERATED_TARGET` avec les préfixes Make documentés.
2. Définir dans `parseTag.c` un chemin relatif unique avec chaque commande de génération.
3. Ajouter à la configuration autoCode un répertoire de sortie obligatoire.
4. Rejeter un chemin absolu, `..`, une cible vide, un doublon ou une sortie hors racine.
5. Produire un manifeste trié contenant tag, propriétaire, point d'inclusion et fichier publié.

Le mapping tag-chemin doit rester dans la table de dispatch, pas être dupliqué entre plusieurs
switchs. Le manifeste devient la source consommable par les contrôles et les outils.

### Étape 2 — Migrer le générateur avant les sources

1. Séparer la lecture des ancres de l'écriture des destinations.
2. Faire écrire chaque fonction `write*()` dans son propre temporaire `.inc`.
3. Adapter `fileCmpReplaceAll()` au cas d'une destination absente.
4. Publier le manifeste en dernier et conserver le nettoyage de tous les temporaires sur erreur.
5. Vérifier que les douze tags existent une seule fois et que leur ligne `#include` correspond au
   chemin attendu ; refuser tout autre contenu entre les deux tags.

Cette étape doit d'abord être validée dans les fixtures sous `test/autoCode/`, sans toucher aux
zones réelles.

### Étape 3 — Remplacer une seule famille pilote

Migrer d'abord les trois tags de catalogue de `sysCall`, qui sont à portée de fichier et sans
inclusion matérielle. Exécuter la génération deux fois, comparer le prétraitement et construire les
deux cibles. Cette tranche valide le format `.inc`, les chemins, les mtimes et les fichiers `.d`
avant de traiter les corps de fonction ou les en-têtes.

### Étape 4 — Migrer les neuf autres tags

1. Migrer les types et constantes d'`interfaces`.
2. Migrer les allocations et appels dans les corps de fonctions.
3. Migrer les agrégations d'en-têtes HAL et sysCore en dernier.
4. Après chaque groupe, vérifier que Git ne montre que la suppression initiale du texte généré et
   l'ajout stable des `#include` suivis.
5. Quand les douze tags utilisent les fragments, supprimer définitivement le chemin de réécriture
   des `.c` et `.h` dans `parseTag()`.

### Étape 5 — Adapter le build et les contrôles

1. Faire exécuter autoCode à chaque build, ou mettre en place le manifeste d'empreintes complet.
2. Ajouter le chemin d'inclusion de la seule pile sélectionnée.
3. Scanner le manifeste dans les contrôles d'architecture et d'en-têtes critiques.
4. Ajouter le nettoyage confiné et l'exclusion Git explicite dans les sources de la `.gitignore`.
5. Adapter `cppcheck`, ctags, l'éditeur, `cloc`, Doxygen et la documentation d'architecture.

### Étape 6 — Étendre les tests avant publication

Le corpus doit ajouter au minimum :

- génération depuis un clone sans répertoire ni fragment ;
- suppression d'un seul `.inc` avec inputs et stamp inchangés ;
- seconde génération à `0 updated` et mtimes inchangés ;
- correction d'un fragment modifié manuellement ;
- erreur tardive laissant tous les fragments publiés précédents inchangés ;
- absence de `.tmp` après succès, erreur ou signal géré ;
- tag absent, dupliqué, mauvais include ou contenu additionnel entre les tags ;
- alternance `test1 -> test_noscli -> test1` sans réutilisation croisée ;
- détection d'un include architectural interdit placé dans un `.inc` ;
- refus du nettoyage de `.`, `srcs/`, `build/` et de tout chemin sortant de `generated/` ;
- fragments ignorés par Git et absents de l'index.

## Critères d'acceptation

La migration est terminée seulement si toutes les conditions suivantes sont satisfaites :

- les onze `.c`/`.h` suivis contiennent seulement un `#include` stable entre les douze tags ;
- aucun lancement d'autoCode ne modifie un fichier suivi par Git ;
- les douze fragments `.inc` sont sous `generated/${VAL_HW_STACK:ts_}` et aucun n'est suivi ;
- un clone propre construit sans étape manuelle et un fragment manquant force sa régénération ;
- les deux piles ne partagent aucun fragment et l'alternance des cibles reste correcte ;
- les contrôles d'architecture et d'en-têtes critiques attribuent les fragments à leur
  propriétaire ;
- les `.d` référencent les `.inc` et une modification recompile seulement les unités concernées ;
- les tests autoCode normaux et ASan/UBSan passent, y compris les nouveaux cas ;
- `bmake clean && bmake` et le build propre `VAL_TARGET=test_noscli` atteignent `Build complete` ;
- flash, SRAM, symboles et désassemblage ne montrent aucune différence fonctionnelle inexpliquée ;
- le boot, l'ordonnancement, SCLI, GPIO, USART, I2C, LCD et RTC sont validés sur Arduino Mega ;
- `git diff --check`, `cppcheck` et les vérifications négatives Git/nettoyage passent.

## Validation réalisée et limites

L'audit a inspecté les douze tags, leurs contextes C, `parseTag.c`, `fileUtility.c`, les règles
`autoCode`, l'ordre du build, la génération des dépendances, la `.gitignore` et les deux contrôles
d'inclusions. `bmake test_autoCode`, `bmake test_autoCode_sanitize` et
`bmake _architecture_include_check` passent sur la révision auditée.

Aucun prototype d'extraction, build AVR comparatif ni essai Arduino Mega n'a été exécuté : ils
appartiennent aux étapes de mise en œuvre et aux critères d'acceptation, pas à cet audit en lecture
seule du firmware.

## Conclusion

L'extraction répond bien au problème Git : les inputs et points d'inclusion restent revus et suivis,
tandis que les sorties variables deviennent des artefacts reconstruits. Le format `.inc` conserve
la sémantique embarquée sans nouvelle allocation, table dynamique ni unité de traduction.

La solution recommandée est donc `generated/<pile>/autocode/<propriétaire>/<tag>.inc`, avec
génération systématique avant compilation et manifeste contrôlé. Une sortie globale, un stamp sans
empreinte ou l'exclusion des fragments des contrôles d'architecture doivent être considérés comme
un blocage de publication, même si le firmware compile.
