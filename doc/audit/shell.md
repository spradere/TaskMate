# Audit du code shell dans les Makefiles

## Périmètre

Cet audit analyse les 16 fichiers Make du dépôt à la révision `ec9d549`, soit 1 091 lignes :

- le `Makefile` principal ;
- les fragments généraux de `mk/` ;
- les fragments de la cible, de la carte, du MCU et de l'architecture AVR8.

L'objectif est de déterminer quels blocs shell gagneraient à être déplacés dans des scripts POSIX
`sh`, en priorité lorsqu'ils contiennent du contrôle de flux, des opérations à état, des écritures
répétées ou du code difficile à tester dans une recette.

Aucun code n'a été modifié pendant l'analyse.

## Conclusion

Une extraction ciblée serait utile, mais une externalisation générale nuirait à la lisibilité du
graphe de build. Trois scripts apporteraient un gain net :

| Priorité | Script proposé | Motivation |
|---|---|---|
| P0 | `backup_usb.sh` | shell procédural long et opérations à état |
| P0 | `write_lines.sh` | écritures et règles presque identiques |
| P1 | `write_build_info.sh` | nombreuses redirections vers le même fichier |
| P2 | `git_version.sh` | traitement Git subtil et fallback incorrect |

Les commandes de compilation, les dépendances, les stamps, les sélections matérielles et les appels
directs aux outils doivent rester dans les Makefiles. Plusieurs duplications restantes devraient
être factorisées avec des règles ou des macros `bmake`, et non avec des scripts shell.

## Extractions recommandées

### P0 — Extraire la sauvegarde USB

La recette `backup` occupe environ 30 lignes dans
[`backup.mk`](../../mk/backup.mk#L57). Elle associe :

- une interaction avec l'utilisateur ;
- la détection du montage par une pipeline `mount | grep` ;
- une branche shell `if/else` ;
- le montage du périphérique ;
- une copie destructive `rsync --delete` ;
- le démontage final.

Ce bloc est le meilleur candidat à une extraction. Un script `scripts/backup_usb.sh` pourrait
recevoir le périphérique, le point de montage, la destination, les répertoires exclus et le fichier
de log.

Le script devrait :

- utiliser exclusivement le shell POSIX ;
- activer `set -eu` ;
- valider tous ses arguments avant le montage ;
- mémoriser s'il a lui-même monté le volume ;
- installer un `trap` afin de démonter ce volume en cas d'échec ;
- ne jamais démonter un volume qui était déjà monté avant son exécution.

La recette Make conserverait le nom de la cible et les variables du build, puis appellerait le
script avec des arguments explicites. Cela rendrait le comportement testable sans charger tout le
système `bmake` et améliorerait la sûreté de la séquence de montage.

### P0 — Factoriser les écritures de listes et de petits fichiers générés

Plusieurs règles écrivent un fichier avec un premier `printf >`, puis une suite de `printf >>` :

- les trois définitions de `taskmate_info.h`
  ([`build.mk`](../../mk/build.mk#L24)) ;
- les options de `autoCode_config`
  ([`build.mk`](../../mk/build.mk#L88)) ;
- les quatre listes `FILE_INITRC_LIST`, `FILE_PARSE_TAG_LIST`, `FILE_HALINIT_LIST` et
  `FILE_HALDEFINE_LIST`
  ([`build.mk`](../../mk/build.mk#L122)).

Les quatre règles de listes sont presque identiques : elles vident une destination, parcourent une
liste avec `.for`, puis ajoutent un chemin par ligne. Un script limité à cette responsabilité
éviterait la duplication :

```text
write_lines.sh <fichier-sortie> <ligne>...
```

Le script devrait produire un fichier temporaire dans le même répertoire, comparer le résultat à la
destination et ne remplacer celle-ci que lorsque le contenu change. Cette stratégie apporterait
trois bénéfices :

- aucune sortie partielle en cas d'échec ;
- moins de redirections et de recettes répétées ;
- aucun changement de timestamp inutile susceptible de relancer une génération ou une compilation.

Les cibles, leurs prérequis et leurs stamps doivent rester dans `build.mk`. Le script ne doit pas
devenir propriétaire du graphe de dépendances.

La configuration autoCode est critique. Si `write_lines.sh` est utilisé pour la produire, chaque
ligne doit rester construite explicitement par Make afin que les options attendues restent visibles
lors d'un audit et dans la sortie de `bmake -n`.

### P1 — Extraire la génération des informations de build

La cible spéciale `.END` effectue quatorze écritures successives dans
[`build.mk`](../../mk/build.mk#L36). Elle collecte la version TaskMate, la date, la cible
matérielle, le compteur de build, la version de Git, le tag et le compilateur.

Un script `scripts/write_build_info.sh` pourrait recevoir :

- le fichier de destination ;
- la version TaskMate ;
- la pile matérielle ;
- le compteur de build ;
- le nom et la version du compilateur.

Il produirait `last_build_info.txt` en une opération atomique et pourrait gérer explicitement
l'absence de tag Git. La partie du résumé imprimée dans le terminal, lignes 52 à 65, devrait rester
dans Make : elle est courte, dépend du type de cible demandé et délègue déjà ses calculs aux scripts
AWK appropriés.

### P2 — Isoler la détection de version Git

La version est calculée par la pipeline suivante dans
[`data.mk`](../../mk/data.mk#L16) :

```sh
git describe --tags | cut -d'-' -f1 | sed 's/^v//' || printf "0.00"
```

Le fallback n'est pas fiable en shell POSIX. Le statut d'une pipeline est celui de sa dernière
commande : si `git describe` échoue mais que `sed` traite avec succès une entrée vide, `printf
"0.00"` n'est pas exécuté.

Un petit script `git_version.sh` rendrait ce contrôle explicite et testable. Les composantes majeure
et mineure pourraient ensuite être extraites avec des modificateurs de chaîne `bmake`, ce qui
supprimerait les deux pipelines `echo | cut` suivantes.

La lecture de `VAL_UPSTREAM` à la ligne 24 mérite le même traitement d'erreur. Une branche dont la
référence distante n'est pas disponible ne devrait pas produire un diagnostic Git à chaque
chargement du Makefile.

## Duplications à résoudre dans Make

### Factoriser les recettes de contrôle des programmes

Les contrôles général et AVR8 utilisent déjà correctement
[`check_programs.sh`](../../scripts/check_programs.sh). Leurs recettes conservent toutefois quatre
lignes presque identiques dans [`build.mk`](../../mk/build.mk#L29) et
[`archCC.mk`](../../srcs/hal/arch/avr8/archCC.mk#L20) : message, appel du script, création du
répertoire et mise à jour du stamp.

Cette duplication devrait être traitée par une règle `.USE`, une macro ou une règle commune `bmake`.
Ajouter le chemin du stamp aux responsabilités du script cacherait la relation entre la cible et ses
prérequis, et n'est donc pas recommandé.

### Factoriser les messages de recettes

La forme suivante apparaît dans de nombreuses cibles :

```make
@printf "\n%sMessage%s\n\n" \
	"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
```

Un script lancé uniquement pour afficher une ligne ajouterait un processus sans simplifier le build.
Une macro Make destinée aux titres de recettes serait plus adaptée.

### Simplifier la génération des tags

[`editors.mk`](../../mk/editors.mk#L16) lance quatre fois `ctags`, dont trois fois en mode ajout.
Sauf contrainte de longueur de ligne de commande, un seul appel avec les quatre listes de sources
suffirait. Il s'agit d'une simplification de recette, pas d'un besoin de script.

## Code à conserver dans les Makefiles

### Compilation et architecture AVR8

Les commandes de compilation et de linkage de
[`archCC.mk`](../../srcs/hal/arch/avr8/archCC.mk#L27) utilisent les variables automatiques de Make,
les options de compilation par fichier et les sorties décrites par le graphe. Les déplacer dans un
script masquerait les paramètres spécifiques AVR et compliquerait l'inspection avec `bmake -n`.

Les appels directs à `avr-size`, `avr-nm`, `avr-objdump` et `avrdude` sont également assez courts.
Seule la règle de construction du HEX doit être factorisée.

### Outils d'analyse et de formatage

Les appels à `cloc`, `cppcheck`, `clang-format` et `clang-tidy` dans
[`utils.mk`](../../mk/utils.mk#L34) sont longs parce qu'ils exposent leurs options. Ils ne
contiennent pas de logique shell complexe. Les conserver dans Make permet de voir immédiatement les
fichiers, les inclusions et les suppressions de diagnostics employés par chaque cible.

Le même raisonnement s'applique à `tidy_TaskMate` dans
[`archCC.mk`](../../srcs/hal/arch/avr8/archCC.mk#L84), dont les paramètres appartiennent à
l'architecture AVR8.

### Découverte des sources

Les commandes `find` de [`sources.mk`](../../mk/sources.mk#L17) alimentent directement des variables
Make utilisées pour les objets, dépendances et entrées autoCode. Un script générique de découverte
cacherait la distinction entre les racines multiples et les chemins scalaires, sans supprimer le
nombre d'exécutions nécessaires.

Les listes multi-racines ne doivent notamment pas être regroupées dans un seul argument shell cité :
chaque racine doit rester un élément distinct de la commande `find`.

### Génération de `.gitignore`

La règle de [`backup.mk`](../../mk/backup.mk#L33) repose sur des boucles `.for` natives de `bmake`
et sur les associations `FILES_GIT_ALLOWED.${dir}`. Il ne s'agit pas d'une boucle shell. La déplacer
dans un script obligerait Make à sérialiser ces associations et affaiblirait la frontière actuelle
où `mk/path_files.mk` reste la source de vérité.

### Vérifications AWK

Les traitements AWK sont déjà externalisés dans `scripts/*.awk`. Les Makefiles ne conservent que les
appels, les arguments dynamiques transmis avec `-v` et les relations de dépendance. Cette séparation
est appropriée et ne nécessite pas de script shell supplémentaire.

### Fichiers déclaratifs

Les fichiers suivants ne contiennent aucun bloc qui gagnerait à être extrait :

- `Makefile` ;
- `mk/options.mk` ;
- `mk/hardware_target.mk` ;
- `mk/path_files.mk`, hors une substitution `printf` simple ;
- `srcs/hal/arch/avr8/arch_make.mk` ;
- `srcs/hal/mcu/atmega2560/mcu_make.mk` ;
- `srcs/hal/board/arduinoMega/board_make.mk` ;
- `srcs/user/target/test1/hwt.mk`.

Ils décrivent des chemins, options, drapeaux, limites matérielles ou inclusions. Ces informations
doivent rester visibles dans les fragments Make correspondant à leur frontière architecturale.

## Optimisations Make sans script

[`colours.mk`](../../mk/colours.mk#L41) lance `printf` 27 fois au chargement pour construire les
codes ANSI. Un script appelé 27 fois ne réduirait ni la duplication ni le coût. Il serait préférable
de calculer une seule fois le caractère d'échappement, puis de construire les couleurs par expansion
de variables Make.

Les commandes simples de `data.mk`, `sources.mk` et `arch_make.mk` qui retournent directement une
valeur à une variable avec `!=` peuvent rester en place lorsqu'elles ne contiennent ni pipeline ni
gestion d'erreur complexe.

## Anomalies connexes

- la pipeline `ls | head | xargs cat` de `autoCode_alone` gère mal l'absence de journal
  ([`build.mk`](../../mk/build.mk#L147)).

Pour le dernier point, la logique doit vérifier explicitement qu'un fichier a été trouvé avant de
lire son contenu. Elle peut rester inline si elle demeure courte ; un script ne se justifie que si
ce comportement est réutilisé ou couvert par des tests dédiés.

## Ordre de mise en œuvre recommandé

1. Extraire `backup_usb.sh` et tester les chemins volume déjà monté, montage réussi et échec de
   `rsync`.
2. Ajouter `write_lines.sh`, migrer les quatre règles de listes, puis la configuration autoCode et
   `taskmate_info.h` si les comparaisons de sorties sont strictement identiques.
3. Ajouter `write_build_info.sh` avec une écriture atomique.
4. Créer la règle `${FILE_HEX}: ${FILE_ELF}` et factoriser les contrôles de programmes avec les
   mécanismes natifs de `bmake`.
5. Traiter séparément la version Git, les cibles `.PHONY` incorrectes et la lecture du dernier
   journal autoCode.

Chaque étape devra être validée par `sh -n`, une comparaison des fichiers générés, `bmake -n` sur
les cibles affectées et une compilation BSD `bmake` complète. La compilation ne remplace pas la
validation sur Arduino Mega pour les workflows `upload` ou les changements d'outillage liés au
firmware.
