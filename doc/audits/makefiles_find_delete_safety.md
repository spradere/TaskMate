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

