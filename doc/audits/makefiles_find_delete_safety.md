# Audit du remplacement de `rm` par `find -delete` dans les Makefiles

Date de l'audit : 13 septembre 2026  
Révision auditée : `b4806f0` (`replace makefiles usage rm -> find -delete`)  
Périmètre principal : `mk/utils.mk`, `mk/autoCode.mk`, `mk/build.mk`

## Conclusion

Le remplacement compile et fonctionne sur plusieurs chemins nominaux, mais il n'est pas suffisamment
sécurisé pour être considéré comme validé. Les tests `[ -n ... ]`, `[ -f ... ]` et `[ -d ... ]` évitent
une partie des expansions vides et des erreurs de type. Ils ne garantissent toutefois ni que la cible
reste dans `build/`, ni que l'opération demandée a réellement été exécutée.

L'audit relève un risque critique de suppression hors du répertoire de construction, deux régressions
fonctionnelles certaines et plusieurs changements de sémantique. La correction prioritaire consiste à
confiner toute suppression à une racine canonique approuvée, puis à rétablir exactement les ensembles de
fichiers supprimés avant le remplacement.

| Priorité | Constat | Effet |
|---|---|---|
| Critique | Les gardes valident l'existence et le type, pas l'appartenance à `build/` | Une surcharge ou une erreur de variable peut supprimer hors du projet |
| Haute | Le nettoyage des logs fournit un chemin complet à `find -name` | Aucun ancien log autoCode n'est supprimé |
| Haute | `clean` ne supprime plus `.autoCode_stamp*` | Le contrat de nettoyage a régressé |
| Moyenne | Le nouveau motif `autoCode_*` est récursif | Des fichiers imbriqués auparavant hors périmètre sont supprimés |
| Moyenne | Les liens symboliques et types inattendus sont ignorés silencieusement | La cible peut réussir en laissant des artefacts |
| Faible | Les recettes sont POSIX `sh`, mais les primitives `find` ne sont pas POSIX | La dépendance BSD/GNU doit être assumée et testée |
| Faible | L'affichage de `clean` ne correspond pas exactement aux suppressions | Le diagnostic opérateur est trompeur |

## Périmètre et méthode

L'analyse compare le commit `b4806f0` à son parent, inspecte les valeurs développées par `bmake`, puis
exécute les expressions `find` dans une arborescence isolée sous `/tmp`. Elle couvre les variables
vides, les chemins nominaux, les motifs, la profondeur, les liens symboliques et les surcharges de
variables en ligne de commande. Une construction AVR réelle a aussi été lancée après `bmake clean`.

Le commit contient également la suppression de passages dans
`doc/audits/files_directories.md` et une modification de `srcs/tm_libc/tm_snprintf.c`. Ces changements
ne relèvent pas du remplacement de `rm` et compliquent la traçabilité de la révision auditée.

## Constats détaillés

### 1. Critique — absence de confinement à la racine de construction

Les gardes de `mk/utils.mk` vérifient seulement que la variable n'est pas vide et que son chemin existe
avec le type attendu. Par exemple, `clean_hard` accepte tout répertoire existant dans
`${PATH_BUILD_TARGET}` avant d'exécuter :

```make
find "${PATH_BUILD_TARGET}" -mindepth 1 -depth -delete
```

Or les variables Make peuvent être surchargées en ligne de commande. Le développement statique de
`bmake -n PATH_BUILD_TARGET=/ clean_hard` contient bien :

```sh
if [ -n "/" ] && [ -d "/" ]; then find "/" -mindepth 1 -depth -delete; fi
```

Le test `-n` empêche le cas vide, mais `/`, `.`, le répertoire du dépôt, un répertoire parent ou un
chemin extérieur restent tous valides. Le même défaut touche les suppressions récursives de `clean` et
la variable `${PATH_BUILDS}` de `clean_hard`. `-mindepth 1` protège seulement la racine elle-même, pas
son contenu.

Il faut résoudre la racine et la cible en chemins canoniques, refuser explicitement `/`, `.`, la racine
du dépôt et toute cible extérieure, puis vérifier que `${PATH_BUILD_TARGET}` est un descendant strict de
la racine de construction approuvée. Une garde refusée doit terminer la cible avec une erreur, pas être
un succès silencieux. Si une racine de build personnalisable est nécessaire, elle doit passer par une
validation explicite distincte d'une simple surcharge accidentelle.

### 2. Haute — le motif de suppression des logs ne peut pas correspondre

Dans `mk/path_files.mk`, `${FILE_AUTOCODE_LOG}` vaut `build/log/autoCode_log`. La recette de
`mk/autoCode.mk` parcourt déjà `${PATH_LOGS}`, mais donne le chemin complet au prédicat `-name` :

```make
find "${PATH_LOGS}" -maxdepth 1 -type f -name "${FILE_AUTOCODE_LOG}*" -delete
```

Sur le `find` FreeBSD utilisé, `-name` compare uniquement le dernier composant du chemin. Aucun nom de
fichier ne peut donc correspondre au motif `build/log/autoCode_log*`. Le test isolé conserve le fichier,
et la construction réelle a laissé côte à côte les logs de `10:58:18` et `14:26:18`.

Le motif doit être limité au nom de base, par exemple `${FILE_AUTOCODE_LOG:T}*`, tout en conservant
`${PATH_LOGS}` comme racine de recherche.

### 3. Haute — perte du nettoyage de `.autoCode_stamp*`

Avant `b4806f0`, `clean` supprimait explicitement
`${PATH_BUILD_TARGET}/.autoCode_stamp*`. La nouvelle recette affiche encore
`${PATH_BUILD_TARGET}/.autoCode_*`, mais ne recherche que `autoCode_*`, sans point initial :

```make
find "${PATH_BUILD_TARGET}" -type f -name "autoCode_*" -delete
```

Après un `bmake clean` réel, `build/test1_arduinoMega_atmega2560_avr8/.autoCode_stamp` était toujours
présent. Le build suivant a tout de même régénéré autoCode parce que l'exécutable hôte avait été
recompilé et était plus récent ; ce succès ne restaure pas le contrat de `clean`.

Il faut réintroduire une suppression explicite du stamp exact ou, si les variantes suffixées sont
intentionnelles, un `-maxdepth 1 -name ".autoCode_stamp*"` documenté.

### 4. Moyenne — extension involontaire de la profondeur

L'ancienne commande `${PATH_BUILD_TARGET}/autoCode_*` ne visait que les entrées directement sous le
répertoire de cible. Le nouveau `find` n'a pas de `-maxdepth 1` et supprime tout fichier `autoCode_*`
dans les sous-répertoires. Le test isolé a ainsi supprimé à la fois `autoCode_root` et
`nested/autoCode_nested`.

Cette extension n'est pas requise par un remplacement équivalent de `rm`. Il faut ajouter
`-maxdepth 1`, sauf si le nouveau périmètre récursif est une décision documentée et couverte par des
tests.

### 5. Moyenne — liens symboliques et états incohérents masqués

Les tests `-f` et `-d` suivent les liens symboliques, tandis que le `find` courant ne les suit pas et
filtre ensuite sur `-type f` ou utilise `-mindepth 1`. Dans les essais isolés :

- un lien vers un fichier satisfait `[ -f ... ]`, mais `find ... -type f -delete` laisse le lien ;
- un lien vers un répertoire satisfait `[ -d ... ]`, mais le nettoyage récursif laisse également le
  lien.

La recette termine alors avec succès sans atteindre son objectif. Les gardes ignorent aussi
silencieusement un chemin existant du mauvais type. Il faut définir une politique explicite pour les
liens symboliques et distinguer « absent, donc rien à faire » de « présent mais incohérent, donc erreur ».
Pour une cible fichier, un `find` borné à `-maxdepth 0` peut inclure explicitement `-type l` si la
suppression du lien est souhaitée.

### 6. Faible — shell POSIX, extensions `find`

Les conditions `[ -n ... ]`, `[ -f ... ]` et `[ -d ... ]` ne sont pas des tests spécifiques à Bash.
Les recettes sont exécutées par `/bin/sh` et leur syntaxe est valide avec ce shell. Parler de
« sécurisation Bash » serait donc imprécis ; il s'agit de gardes shell POSIX.

En revanche, `-delete`, `-mindepth` et `-maxdepth` sont des extensions non POSIX de `find`. Elles sont
présentes sur le FreeBSD courant et couramment disponibles avec GNU find. Le projet déclare déjà
`find` dans `conf/programs-list.conf`, mais le simple contrôle de présence ne vérifie pas ces options.
La dépendance à une variante BSD/GNU compatible doit être documentée ou testée au démarrage.

### 7. Faible — sortie opérateur inexacte

L'affichage de `clean` contient `/**/*.d` au lieu de répéter `${PATH_BUILD_TARGET}` et n'ajoute pas de
retour à la ligne après `${FILE_AUTOCODE_TARGET}`. Deux lignes sont donc concaténées. Il affiche aussi
`.autoCode_*` alors que la commande recherche `autoCode_*`. Pour une opération destructive, le résumé
doit refléter exactement la racine, la profondeur et les motifs utilisés.

## Points corrects du changement

- Les expansions utilisées comme chemins sont placées entre guillemets doubles.
- Les suppressions de contenu complet emploient `-mindepth 1`, ce qui évite de supprimer la racine de
  recherche elle-même.
- Les sélections de fichiers placent `-delete` à la fin de l'expression.
- `find` est déjà une dépendance déclarée du build.
- Les recettes développées sont valides avec `/bin/sh` sur l'environnement courant.
- `bmake clean && bmake` a atteint `Build complete` pour
  `test1 / arduinoMega / atmega2560 / avr8` : Flash 23,8 %, RAM 22,2 %.

Ces points valident l'intégration nominale, mais pas le confinement des suppressions ni l'équivalence
fonctionnelle avec les commandes remplacées.

## Plan de correction recommandé

1. Ajouter une validation centralisée et canonique des racines destructives avant tout `find -delete`.
2. Faire échouer la cible si une variable est vide, dangereuse, extérieure à la racine approuvée ou de
   type incohérent ; conserver seulement l'absence normale comme cas sans action.
3. Corriger le motif de logs avec le nom de base de `${FILE_AUTOCODE_LOG}`.
4. Restaurer la suppression de `.autoCode_stamp*` et ajouter `-maxdepth 1` au motif `autoCode_*` pour
   retrouver le périmètre antérieur.
5. Définir et tester la politique relative aux liens symboliques.
6. Corriger l'affichage de `clean` et documenter la dépendance aux extensions BSD/GNU de `find`.
7. Ajouter un test de nettoyage dans un répertoire temporaire avec manifeste avant/après : cas nominal,
   variable vide, `/`, `.`, chemin parent, chemin extérieur, espaces, métacaractères, mauvais type,
   cible absente et liens symboliques.
8. Séparer les modifications Makefile des changements C et documentation non liés afin de permettre
   une revue et un retour arrière ciblés.

## Validation effectuée

- comparaison de `b4806f0` avec son parent ;
- inventaire des commandes `rm` avant modification et des `find -delete` actuels ;
- inspection de `bmake -V` pour les chemins concernés ;
- inspection de `bmake -n clean clean_hard` et `bmake -n autoCode_alone` ;
- développement de `bmake -n PATH_BUILD_TARGET=/ clean_hard`, sans exécution destructive ;
- essais isolés des motifs, profondeurs et liens symboliques sous `/tmp` ;
- `bmake clean`, suivi d'un inventaire des stamps et logs restants ;
- `bmake` réel terminé avec `Build complete` ;
- `git diff --check` sans erreur avant création du présent rapport ;
- aucun essai sur matériel Arduino Mega physique.
