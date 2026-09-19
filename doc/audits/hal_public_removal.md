# Audit de suppression de `hal/public`

Date : 19 septembre 2026

Branche auditée : `test`

Révision auditée : `6f0583f`

Révision du premier audit : `04f0603`

## Objet et verdict

Cet audit réévalue la suppression de `srcs/hal/public/` après les refactorings de la
sélection des sources, des chaînes et du GPIO. Il couvre les six relais restants, leurs douze
inclusions directes dans dix fichiers, les contrats neutres, autoCode et la composition BSD
`bmake`.

La migration a nettement avancé : l'agrégateur `hal_define`, les relais de constantes, de chaînes
et de GPIO, ainsi que le passage GPIO par sysCore ont disparu. Les fichiers `init.rc` sélectionnent
désormais les unités de traduction des modules. La façade restante ne contient toujours aucune
implémentation `.c`.

La suppression est **faisable, mais pas encore prête**. Les six fichiers restants se répartissent
en trois groupes de difficulté très différente :

1. `hal_ports_pins.h` est devenu un relais interne au seul backend ATmega2560 et peut être retiré
   sans créer de contrat transversal ;
2. `hal_atomic.h` et une partie de `hal_architecture_types.h` portent encore l'état atomique
   AVR8 jusque dans `sysCall` ;
3. `hal_context.h`, `hal_stack.h`, `hal_interrupt.h` et le reste des types d'architecture exposent
   au noyau une séquence de changement de pile et de restauration de contexte qui doit rester
   indivisible.

Le chemin cible reste le suivant :

```text
code portable -> interfaces/<capacité>.h -> symbole HAL
                                            ^
                                            |
                         fichier .c/.S choisi par target/board/MCU/arch.mk
```

`interfaces/` décrit le contrat. Il ne doit contenir ni registre, ni assembleur, ni type physique
de broche, ni sélection `ARCH_*`/`MCU_*`. Le build choisit l'implémentation ; le préprocesseur ne
doit plus choisir un en-tête concret pour le code portable.

## Progression depuis le premier audit

| Sujet du premier audit | État au 19 septembre 2026 | Conséquence |
| --- | --- | --- |
| Sélection fine des sources | Partiellement réalisée | Les modules viennent de `-source_file`/`-source_dir`, mais AVR8 et `tmLibc` sont encore ajoutés par répertoire. |
| `hal_define` et quatre `define.h` | Réalisé | Le tag, l'option, les listes Make et le relais ont disparu. |
| Constantes d'architecture et de périphérique | Réalisé | Elles sont consommées localement par les backends. |
| Relais de chaînes `hal_string.h` | Réalisé pour la façade | Les macros AVR8 sont maintenant injectées par le build ; la dette `tmLibc`/`sysCall` vers HAL reste distincte. |
| Relais GPIO `hal_gpio.h` | Réalisé | Le contrat logique est dans `interfaces/gpio_signals.h` et `sysCall` appelle directement le HAL. |
| Descripteurs physiques hors du système | Presque réalisé | Ils sont confinés au MCU et au câblage cible, mais passent encore par `hal_ports_pins.h`. |
| Contrat atomique neutre | Non réalisé | `sysCall` inclut toujours le backend AVR8 à travers deux relais. |
| Démarrage de contexte indivisible | Non réalisé | `sys_scheduler.c` compose encore quatre primitives inline AVR8. |
| Représentation neutre de pile | Non réalisé | `mod_thread_item_t` expose toujours `hal_stack_word_t`. |
| Suppression et durcissement final | Non commencé | Le chemin, les gardes et la documentation décrivent encore `hal/public`. |

Les occurrences de `haldefine`, `hal_init`, `hal_fxinit` et `hal_define` qui subsistent dans
`test/autoCode/autoCode_test.sh` sont volontaires : elles vérifient que les anciennes options et
anciens tags sont rejetés. Elles ne constituent pas des fonctionnalités actives à supprimer.

## État exact de la façade restante

| Relais | Backend sélectionné | Inclus par | Diagnostic |
| --- | --- | --- | --- |
| `hal_architecture_types.h` | `avr8_architecture_types.h` | cinq fichiers HAL/sysCore | Mélange `hal_stack_word_t` et `hal_atomic_state_t`; c'est le nœud commun des deux contrats critiques. |
| `hal_atomic.h` | `avr8_atomic.h` | `sc_driver.c`, `sc_threads.c` | Sauvegarde `SREG`, exécute `cli`, puis restaure exactement l'état précédent. |
| `hal_context.h` | `avr8_context.h` | `sys_modules.c`, `sys_scheduler.c` | Expose l'initialisation de contexte, les macros d'assembleur et la restauration inline. |
| `hal_interrupt.h` | `avr8_interrupt.h` | `sys_scheduler.c` | Expose `sei`, `cli` et `reti` séparément. |
| `hal_ports_pins.h` | `at2560_ports_pins.h` | `at2560_gpio.h` | Sélection MCU inutile entre deux fichiers du même backend. |
| `hal_stack.h` | `avr8_stack.h` | `sys_scheduler.c` | Expose l'écriture de `SP` comme primitive isolée. |

Les douze inclusions directes sont réparties dans dix fichiers. Sept inclusions appartiennent au
système (`sysCall` ou sysCore) et cinq à HAL lui-même. La façade ne joue donc plus aucun rôle pour
les pilotes, les services, les tâches ou `tmLibc`.

## Composition actuelle du build

La chaîne sélectionnée reste :

```text
test1 -> arduinoMega -> atmega2560 -> avr8
```

`FILES_COMPILE_SRC` n'est plus construit en parcourant tous les répertoires enregistrés. Pour
`test1`, il contient 29 unités de traduction :

- les services, pilotes et tâches nommés dans `test1_init.rc` ;
- les commandes SCLI demandées par son unique `-source_dir` ;
- `at2560_gpio.c`, ajouté explicitement par le fragment MCU ;
- les sources de base de sysCore et `sysCall` ;
- les deux sources AVR8 et les trois sources `tmLibc`, encore trouvées par répertoire.

Le premier objectif de la feuille de route historique est donc majoritairement atteint : LCD et
RTC ne sont plus compilés uniquement parce que `hal/drivers` est dans un chemin global. Il reste
toutefois quatre limites :

1. `PATHS_EXTRA_SRC` ajoute encore les répertoires complets `hal/arch/avr8` et `tmLibc` ;
2. le build ne vérifie pas explicitement qu'une capacité possède exactement une implémentation ;
3. la sélection d'en-têtes se fait encore par les `#if ARCH_avr8`/`MCU_atmega2560` de la façade ;
4. `test_noscli` est annoncé dans `conf/hardware-targets.conf`, mais son `target.mk` n'existe plus.

Le dernier point invalide l'ancien critère comparatif `test1`/`test_noscli`. Il faut soit restaurer
une vraie cible `test_noscli`, soit la retirer de la liste des cibles prises en charge et remplacer
ce test par une fixture de composition.

## Refactorings déjà exploitables

### GPIO logique

`interfaces/gpio_signals.h` porte maintenant `gpio_signal_t` et les trois opérations HAL logiques.
`sc_gpio.c` les appelle sans passer par un stockage sysCore. Le backend ATmega2560 possède
`signal_table`, les registres de ports et les types physiques. autoCode inclut le câblage cible
dans l'unité de traduction du MCU au moyen de `wire_gpio.inc`.

Cette direction permet de supprimer immédiatement `hal_ports_pins.h` : `at2560_gpio.h` peut inclure
son en-tête MCU local, sans publier ces types dans `interfaces/`. L'inclusion de
`hal_architecture_types.h` par `at2560_gpio.c` est par ailleurs inutilisée et peut disparaître.

Avant de considérer le contrat stabilisé, il faut toutefois :

- rendre `interfaces/gpio_signals.h` autonome en incluant explicitement `<stdbool.h>` ;
- restaurer le contrôle autoCode qui refuse un `--wire_gpio` hors de `--source_path` ;
- valider les indices de signal, port et broche, ainsi que la complétude du câblage ;
- décider et appliquer la polarité `active_high`, aujourd'hui stockée mais ignorée ;
- documenter que le toggle lecture-écriture n'est pas atomique.

Ces points sont des exigences de robustesse du nouveau chemin, pas une raison de remettre un
relais public MCU.

### Agrégateur et autoCode historiques

`hal_define.h`, `FILES_HALDEFINE`, `--haldefine` et le tag `hal_define` ont disparu. Les anciens
tags d'initialisation HAL ont eux aussi été retirés. La composition des modules et des sources est
désormais portée par `init.rc` et par les fragments de cible.

La dernière étape de l'ancien audit consacrée à leur suppression est donc close et ne doit pas
être rejouée.

### Chaînes et mémoire programme

Le relais `hal_string.h` a disparu. `avr8_string_macro.h` fournit maintenant les littéraux
`PROGMEM` et `pgm_read_byte`, injectés par les options de compilation dans le système et
`tmLibc`.

Cela ne bloque plus la suppression de `hal/public`, mais ne constitue pas encore la frontière v10
visée : `tmLibc` reçoit toujours un en-tête HAL par `-include`, et `sc_string.c` utilise une macro
AVR. Cette dette doit être suivie dans le chantier `tmLibc`/chaînes ; elle ne doit pas être
réintroduite dans la façade à supprimer.

## Bloqueurs restants

### 1. Contrat atomique et représentation de l'état

`hal_atomic_state_t` vaut actuellement `uint8_t`, car il contient `SREG`. Une section atomique
sauvegarde l'état, désactive les interruptions, puis restaure la valeur exacte. Remplacer cette
sémantique par une paire globale `disable/enable` casserait l'imbrication et pourrait réactiver les
interruptions dans un appelant qui les avait déjà masquées.

Le contrat neutre doit donc spécifier :

- la restauration exacte de l'état antérieur ;
- l'absence de fenêtre d'interruption entre la capture et le masquage ;
- le comportement autorisé en contexte ISR ;
- le coût maximal en pile et en instructions sur AVR8.

La représentation ne doit pas être choisie par commodité. `uintptr_t` coûte deux octets sur AVR8
alors que `SREG` n'en demande qu'un ; `uint8_t` n'est pas garanti suffisant pour une future
architecture 16/32 bits. Il faut prototyper le type neutre ou une API qui masque le jeton, mesurer
les appels avec et sans LTO, puis seulement introduire par exemple `interfaces/hal_atomic.h`.

### 2. Pile et initialisation des threads

`mod_thread_item_t` stocke un pointeur et un tableau de `hal_stack_word_t`. Le type concret vaut un
octet sur AVR8 et fixe directement la RAM statique de chaque thread. Le déplacer tel quel dans
`interfaces/` prétendrait à tort qu'une pile matérielle est toujours un tableau d'octets sans
contrainte d'alignement.

Le futur contrat doit séparer :

- le stockage statique réservé par le noyau ;
- son unité et son alignement garantis au backend ;
- le pointeur de pile sauvegardé ;
- l'initialisation de la trame initiale par `hal_threadContextInit()`.

Une solution fondée sur un buffer d'octets et un pointeur opaque est plausible, mais elle doit
être validée sur l'alignement, `sizeof(mod_thread_item_t)`, `.bss`, les canaris et le code généré.
La migration ne doit ni doubler les piles AVR8 ni introduire d'allocation dynamique.

### 3. Démarrage du premier contexte

`tm_schedulerStart()` exécute actuellement, dans cet ordre :

```text
charger SP -> restaurer R0..R31/SREG -> sei -> reti
```

Les quatre étapes sont exposées comme fonctions `always_inline` séparées. Elles forment pourtant
une seule transition matérielle qui ne retourne pas. Les convertir en appels C externes
indépendants ajouterait des adresses de retour ou des prologues sur une pile en cours de
remplacement.

Le contrat portable doit exposer une unique opération de démarrage `noreturn`. Le backend AVR8
doit posséder la séquence complète. `AVR8_CONTEXT_SAVE`, `AVR8_CONTEXT_RESTORE`, `SP`, `SREG`,
`sei`, `cli` et `reti` restent privés à l'architecture et au timer d'ordonnancement. La correction
doit être vérifiée au désassemblage, pas seulement par un build réussi.

## Feuille de route mise à jour

### Étape 0 — Rétablir une base de validation fiable

1. Mettre à jour `test_build_configuration` pour la composition actuelle :
   `PATHS_INITRC_SOURCES` vaut maintenant `system/services/commands`, pas `hal/arch/avr8`.
2. Restaurer le rejet de `--wire_gpio` lorsque le fichier sort de `--source_path` et conserver le
   test négatif existant.
3. Corriger l'agrégation des `.d` pour que deux invocations Cygwin/Windows successives de `bmake`
   acceptent le fichier `.deps.d` sans erreurs liées aux `CRLF`.
4. Décider du sort de `test_noscli` : cible restaurée et testée, ou entrée supprimée de la matrice.
5. Conserver ELF, map, `avr-size`, désassemblage et mesures de pile comme référence avant les
   changements de contexte.

### Étape 1 — Retirer le reliquat GPIO de la façade

1. Faire inclure `at2560_ports_pins.h` directement par le backend ATmega2560.
2. Supprimer l'inclusion inutilisée de `hal_architecture_types.h` dans `at2560_gpio.c`.
3. Supprimer `hal_ports_pins.h` et vérifier qu'aucun type physique ne remonte dans le système ou
   `interfaces/`.
4. Corriger l'autonomie du contrat GPIO et valider la génération `wire_gpio.inc`.

Cette étape est locale au MCU et ne doit pas attendre la décision sur les piles.

### Étape 2 — Fermer le contrat atomique

1. Écrire la sémantique neutre avant le type concret.
2. Prototyper et mesurer la représentation de l'état sur AVR8.
3. Introduire le contrat sous `interfaces/` et migrer `sc_driver.c` et `sc_threads.c`.
4. Garder `SREG`, `cli` et les attributs compilateur dans `avr8_atomic.h` ou une implémentation
   AVR8 sélectionnée explicitement.
5. Supprimer `hal_atomic.h` lorsque les deux consommateurs ne voient plus le backend concret.

### Étape 3 — Fermer le contrat contexte/pile

1. Choisir le stockage et l'alignement de pile après mesure de la RAM AVR8.
2. Migrer `mod_thread_item_t` vers cette représentation sans modifier les régions autoCode à la
   main.
3. Exposer dans `interfaces/` l'initialisation du contexte et une unique opération de démarrage
   `noreturn`.
4. Déplacer toute la séquence de démarrage dans le backend AVR8.
5. Vérifier canaris, préemption, yield, imbrication atomique et désassemblage.
6. Supprimer `hal_context.h`, `hal_stack.h`, `hal_interrupt.h` et, une fois ses deux types migrés,
   `hal_architecture_types.h`.

### Étape 4 — Achever la sélection au build

1. Remplacer les deux ajouts par répertoire dans `PATHS_EXTRA_SRC` par des listes de fichiers
   explicites ou des capacités sélectionnées.
2. Ajouter des diagnostics précoces pour une implémentation absente, en double ou incompatible.
3. Conserver les répertoires de recherche uniquement pour les en-têtes, erreurs, tags et outils,
   pas comme source implicite de compilation.
4. Ajouter aux tests de composition des fixtures avec et sans SCLI et avec un pilote omis.

### Étape 5 — Supprimer le répertoire et durcir les frontières

1. Supprimer les six relais restants puis `srcs/hal/public/`.
2. Retirer le chemin de `PATHS_SOURCE_SEARCH`, les entrées de
   `conf/system_header_allow.conf` et les gardes `HAL_PUBLIC_*`.
3. Adapter `doc/architecture/hal.md`, `doc/architecture/gpio.md`,
   `doc/rules/TaskMate_prefixes.md` et la carte d'architecture si nécessaire.
4. Interdire les inclusions concrètes `hal/arch`, `hal/mcu` et `hal/board` depuis `system`,
   `tmLibc`, `interfaces`, services et tâches. Les fichiers HAL et la configuration cible restent
   les seuls propriétaires autorisés.
5. Ajouter une recherche négative sur `hal/public`, `HAL_PUBLIC_` et les anciens relais, en
   distinguant les tests explicites de compatibilité négative.

## Critères d'acceptation

La migration est terminée uniquement si toutes les conditions suivantes sont satisfaites :

- `srcs/hal/public/` n'existe plus et aucun include actif ne le référence ;
- le système ne voit que des contrats neutres pour les atomiques, le contexte et le GPIO ;
- `interfaces/` n'inclut aucun fichier HAL et ne contient ni registre, ni nom AVR, ni sélection
  `ARCH_*`/`MCU_*` ;
- les types de ports, broches et registres restent dans le backend MCU et la configuration cible ;
- `FILES_COMPILE_SRC` est composé à partir de fichiers explicitement demandés ;
- chaque cible annoncée dans `hardware-targets.conf` possède un `target.mk` et passe le contrôle ;
- une capacité absente ou fournie deux fois échoue avant l'édition de liens ;
- `bmake test_build_system`, `bmake test_autoCode` et `bmake test_autoCode_sanitize` passent ;
- deux builds successifs réutilisent les dépendances sans erreur de fin de ligne ;
- un build propre et un build incrémental produisent le même graphe de sources ;
- flash, RAM statique et piles restent dans les seuils fixés avant migration ;
- le désassemblage confirme l'absence de prologue, d'épilogue ou de fenêtre d'interruption dans
  le démarrage de contexte ;
- les essais Arduino Mega confirment boot, préemption, yield, atomiques imbriquées, GPIO, USART,
  timers, LCD/RTC et halt terminal.

## Risques et ordre de livraison

Le risque le plus élevé reste la restauration du premier contexte, suivi de la représentation de
pile et des atomiques. Le relais de broches MCU est à faible risque. La composition Make est
fonctionnellement avancée, mais ses tests et ses dépendances incrémentales doivent redevenir fiables
avant de servir de filet de sécurité.

L'ordre de livraison recommandé est donc : base de validation, reliquat GPIO, atomiques,
contexte/pile, sélection explicite finale, puis suppression du répertoire et durcissement. Chaque
livraison doit conserver un firmware constructible et ne doit introduire ni dispatch dynamique,
ni allocation dynamique, ni table de callbacks en RAM, ni `#if` matériel dans le code portable.

Faute de seconde architecture implémentée, la portabilité restera une propriété de structure et de
contrats. Les mesures AVR8 et les diagnostics de composition du build sont donc les preuves
minimales requises en attendant un second backend.

## Validation effectuée pour cette mise à jour

- lecture des six relais restants et de leurs backends AVR8/ATmega2560 ;
- recensement de douze inclusions directes dans dix fichiers ;
- comparaison des changements entre `04f0603` et `6f0583f` ;
- inspection de `sources.mk`, `path_files.mk`, `autoCode.mk`, des fragments cible/carte/MCU/arch,
  des règles d'accès et des tests ;
- évaluation de `FILES_COMPILE_SRC`, `FILES_INITRC_SRC`, `PATHS_INITRC_SOURCES`,
  `FILES_EXTRA_SRC`, `PATHS_EXTRA_SRC` et `FILES_PARSE_TAG` avec BSD `bmake` ;
- constat de 29 sources compilées pour `test1` ;
- constat de l'échec de `VAL_TARGET=test_noscli` : `test_noscli/target.mk` absent ;
- production d'un ELF `test1` pendant la session, avec 15 668 octets de flash sur 65 536 et
  1 825 octets de RAM statique sur 8 192 ;
- exécution directe du corpus autoCode : échec au cas `wire_gpio_outside_source`, la commande
  étant acceptée alors que le test attend son rejet ;
- exécution du test de configuration du build : arrêt sur l'attente obsolète
  `PATHS_INITRC_SOURCES=srcs/hal/arch/avr8`, alors que la valeur actuelle est
  `srcs/system/services/commands` ;
- nouvelle invocation de `bmake test_autoCode` : échec avant le test, car `.deps.d` contient des
  continuations `CRLF` que `bmake` signale comme lignes invalides.

Aucun essai matériel, aucune mesure de profondeur de pile et aucune validation du désassemblage
n'ont été effectués pendant cette mise à jour. Aucun code firmware ni fichier généré versionné n'a
été modifié.
