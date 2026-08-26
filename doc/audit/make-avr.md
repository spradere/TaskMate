# Audit de l'isolation AVR dans le build

## Périmètre

Cet audit analyse, à la révision `c9baa05`, la séparation entre :

- l'infrastructure de build portable de `Makefile` et `mk/*.mk` ;
- les fragments de sélection de la cible, de la carte et du MCU ;
- le backend de compilation AVR8 sous `srcs/hal/arch/avr8/` ;
- les scripts appelés par les recettes de build AVR.

La cible disponible est `test1 / arduinoMega / atmega2560 / avr8`. L'objectif est de vérifier que
les fragments centraux ne contiennent plus de commandes ou d'options AVR et que les mécanismes
spécifiques à la toolchain AVR sont concentrés dans le backend d'architecture.

L'analyse est statique et complétée par un `bmake -n all`. Aucun fichier source ou Makefile n'a été
modifié et aucun firmware n'a été compilé pendant l'audit.

## Conclusion

Les fragments centraux `mk/*.mk` ne contiennent plus de code AVR direct. Ils sélectionnent la cible,
découvrent les sources et appellent des points d'extension fournis par le backend actif sans citer
`avr-gcc`, `avrdude`, le format Intel HEX, un MCU AVR ou une option propre à AVR.

En revanche, l'isolation stricte du build AVR n'est pas terminée :

| Priorité | Emplacement | Écart |
|---|---|---|
| P1 | `hal/mcu/atmega2560/mcu_make.mk` | construction directe de l'option AVR `-mmcu` |
| P1 | `hal/board/arduinoMega/board_make.mk` | construction directe de la macro avr-libc `F_CPU` |
| P1 | `hal/arch/avr8/archCC.mk` | ATmega2560, `avr6` et chemins hôte codés en dur |
| P2 | `scripts/avr_memory_*.awk` | traitements AVR hors du répertoire d'architecture |

La bonne frontière n'est pas de déplacer toutes les valeurs matérielles dans l'architecture. La
carte doit rester propriétaire de sa fréquence et de sa configuration de programmation, et le MCU de
son identité et de ses capacités mémoire. Le backend AVR doit en revanche être seul responsable de
la traduction de ces valeurs en options `avr-gcc`, `avr-objdump` et `avrdude`.

## Infrastructure centrale `mk/*.mk`

### Sélection de cible

[`hardware_target.mk`](../../mk/hardware_target.mk#L15) construit le chemin du fragment `hwt.mk` à
partir de `HWT`, vérifie son existence, puis l'inclut. Il ne connaît ni AVR8, ni l'ATmega2560, ni
l'Arduino Mega.

La chaîne de sélection concrète est correctement portée par les fragments de cible :

```text
user/target/test1/hwt.mk
  -> hal/board/arduinoMega/board_make.mk
     -> hal/mcu/atmega2560/mcu_make.mk
        -> hal/arch/avr8/arch_make.mk
```

Le `Makefile` principal charge finalement le backend de compilation au travers de la variable
`FILE_ARCH_CC` ([Makefile](../../Makefile#L43)). Il ne contient pas de chemin AVR en dur.

### Graphe de build

La cible `all` de [`build.mk`](../../mk/build.mk#L69) dépend de `${FILE_TARGET}` et du point
d'extension `_mcu_memory_data`. Le fragment central ne définit ni la commande de linkage, ni le
format du firmware, ni l'outil de mesure mémoire. Ces responsabilités sont fournies par le backend
sélectionné.

Le résumé appelle seulement
[`build_summary_memory.awk`](../../scripts/build_summary_memory.awk#L12), qui lit un format de données
neutre composé d'un nom, d'une taille utilisée, d'une capacité et d'un pourcentage. Il ne dépend pas
du format de sortie d'`avr-size`.

### Chemins et découverte des sources

[`path_files.mk`](../../mk/path_files.mk#L15) construit les chemins d'artefacts à partir de la pile
matérielle et déclare des noms génériques tels que `FILE_TARGET`, `FILE_MEMRAW` et `FILE_MEMDATA`. Il
ne force ni `.elf`, ni `.hex`, ni `.eep`.

[`sources.mk`](../../mk/sources.mk#L17) découvre les fichiers dans `PATHS_SOURCES`. Les racines de la
carte, du MCU et de l'architecture sont ajoutées par leurs fragments respectifs ; aucun répertoire
AVR n'est ajouté par le code central.

### Contrôle des programmes

Le contrôle central de [`build.mk`](../../mk/build.mk#L29) consomme le fichier désigné par
`FILE_PROGRAMS_LIST`. La liste portable
[`conf/programs-list.conf`](../../conf/programs-list.conf#L1) ne contient aucun outil AVR.

Le second contrôle est enregistré par
[`archCC.mk`](../../srcs/hal/arch/avr8/archCC.mk#L14) et consomme la liste AVR
[`programs.list`](../../srcs/hal/arch/avr8/programs.list#L1). `avr-gcc`, `avr-nm`, `avr-objcopy`,
`avr-objdump`, `avr-size` et `avrdude` sont donc contrôlés uniquement lorsque l'architecture AVR8 est
sélectionnée.

## Écarts de frontière

### P1 - Construction de `-mmcu` dans le fragment MCU

[`mcu_make.mk`](../../srcs/hal/mcu/atmega2560/mcu_make.mk#L26) déclare correctement
`VAL_MCU_SERIAL = atmega2560`, mais construit lui-même :

```make
CFLAGS += -mmcu=${VAL_MCU_SERIAL}
```

La valeur `atmega2560` appartient au MCU. En revanche, `-mmcu` est une option de la toolchain AVR et
devrait être ajoutée par `arch_make.mk`. Cette distinction permettrait à un autre backend de
compiler un MCU à partir de la même classe de métadonnées sans imposer la syntaxe d'`avr-gcc` à la
couche MCU.

Les limites `VAL_FLASH_SIZE_K` et `VAL_RAM_SIZE_K` doivent rester dans ce fragment : elles décrivent
les capacités et les restrictions réelles de l'ATmega2560, pas la toolchain AVR en général.

### P1 - Construction de `F_CPU` dans le fragment board

[`board_make.mk`](../../srcs/hal/board/arduinoMega/board_make.mk#L24) déclare correctement la
fréquence de la carte, mais la transforme directement en option compilateur :

```make
VAL_CPU_FREQ = 16000000UL
CFLAGS += -DF_CPU=${VAL_CPU_FREQ}
```

`VAL_CPU_FREQ` doit rester une donnée board. La macro `F_CPU` est en revanche un contrat de la
toolchain et d'avr-libc ; sa construction doit être effectuée par le fragment AVR.

`VAL_PROGRAMMER = avrispmkII` et `VAL_PROGRAMMER_PORT` sont des données de programmation liées à
la cible et à l'hôte. Elles peuvent rester configurables hors du backend. La commande qui les
interprète avec `avrdude` est, elle, correctement située dans `archCC.mk`.

### P1 - Dépendances ATmega2560 dans le backend AVR8

Le backend d'architecture contient plusieurs hypothèses plus spécifiques qu'AVR8 :

- la cible `dump` force la machine `avr6`
  ([archCC.mk](../../srcs/hal/arch/avr8/archCC.mk#L76)) ;
- `tidy_TaskMate` force `__AVR_ATmega2560__=1`
  ([archCC.mk](../../srcs/hal/arch/avr8/archCC.mk#L84)) ;
- cette même cible impose les chemins `/usr/local/avr/include`, GCC AVR `14.2.0` et des chemins
  absolus `/root/code/TaskMate/current`.

Un autre MCU AVR ou une autre installation de la toolchain ne peut donc pas réutiliser ce fragment
sans le modifier. Le MCU doit fournir la variante de machine nécessaire au désassemblage, tandis que
les chemins d'inclusion doivent provenir de la toolchain sélectionnée ou de variables explicitement
configurables. Les macros prédéfinies du MCU devraient, si possible, être obtenues par l'invocation
normale du compilateur avec l'option MCU plutôt que dupliquées manuellement.

### P2 - Scripts de traitement mémoire AVR hors du backend

[`avr_memory_data.awk`](../../scripts/avr_memory_data.awk#L12) interprète explicitement le format
GNU AVR de `text`, `data` et `bss` pour calculer Flash et RAM.
[`avr_memory_show.awk`](../../scripts/avr_memory_show.awk#L12) est également déclaré comme un
affichage AVR.

Ces scripts ne fuient pas dans `mk/*.mk` : seul `archCC.mk` les appelle. Ils restent toutefois du
code de build AVR placé dans le répertoire global `scripts/`. Si l'objectif est que tout le backend
AVR soit physiquement autonome, ils devraient être déplacés sous `srcs/hal/arch/avr8/`, avec leurs
chemins construits à partir de `PATH_AVR8`.

## Répartition recommandée

```text
mk/*.mk
  orchestration, chemins génériques, découverte, points d'extension

user/target/<HWT>/hwt.mk
  composition BOARD / MCU / ARCH et configuration propre à la cible

hal/board/<BOARD>/board_make.mk
  fréquence et paramètres physiques ou de programmation de la carte

hal/mcu/<MCU>/mcu_make.mk
  identité MCU, capacités mémoire et variante matérielle

hal/arch/avr8/*.mk
  avr-gcc, options -mmcu/F_CPU, linkage, ELF/HEX, avr-size,
  avr-objcopy, avr-objdump, avrdude et adaptation des valeurs MCU/board
```

Cette répartition conserve les données au niveau matériel qui les définit, tout en concentrant la
syntaxe et les outils AVR dans le backend d'architecture.

## Vérifications effectuées

La recherche a couvert tous les fichiers `*.mk`, le `Makefile`, les listes de programmes, les
configurations et les scripts AWK avec les marqueurs suivants :

```text
avr-gcc, avr-size, avr-nm, avr-objcopy, avr-objdump, avrdude,
avrisp, __AVR, avr6, -mmcu, F_CPU, ihex, .eeprom, AVR8
```

`bmake -V` confirme la sélection suivante :

```text
FILE_ARCH_CC = srcs/hal/arch/avr8/archCC.mk
CC = avr-gcc
VAL_HW_STACK = test1 arduinoMega atmega2560 avr8
```

Enfin, `bmake -n all` montre que le contrôle portable est chargé depuis
`conf/programs-list.conf`, que le contrôle AVR est ajouté depuis
`srcs/hal/arch/avr8/programs.list`, et que les commandes de compilation, linkage et mesure mémoire
AVR proviennent du backend sélectionné. Le dry-run ne remplace pas une compilation, mais il suffit ici
à vérifier l'expansion et l'origine des recettes sans modifier les artefacts du build.
