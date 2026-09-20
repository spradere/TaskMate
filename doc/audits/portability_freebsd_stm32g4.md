# Audit de portabilité — FreeBSD/amd64 et STM32G4/Nucleo

Date de l'audit : 2026-09-20  
Révision analysée : `25747c4` (`tag, update layers view`)

## Périmètre et verdict

Cet audit évalue deux nouveaux targets sans modifier le code :

- une simulation fonctionnelle native sur FreeBSD/amd64 avec `ucontext` ;
- un portage embarqué sur un STM32G4 Cortex-M4F et une carte Nucleo.

Il actualise l'analyse FreeBSD antérieure sans modifier ce
[snapshot historique](amd64_freebsd_ucontext.md).

Les deux portages sont **faisables**, mais aucun n'est un simple changement de compilateur. Le verdict
est **GO conditionnel**. Il faut d'abord rendre les contrats de contexte et de pile réellement
indépendants de l'AVR, corriger les hypothèses d'ABI 16 bits dans le formatage, puis ajouter chaque
pile matérielle dans les couches `arch`, `mcu`, `board` et `target` existantes.

Le port FreeBSD est le chemin le plus court vers des tests fonctionnels reproductibles. Le STM32G4
est le meilleur test de la portabilité embarquée et du temps réel, mais exige une validation sur
carte, un startup/linker script et un changement de contexte Cortex-M soigneusement vérifié.

## Frontières déjà favorables

- [`hal_context_t`](../../srcs/interfaces/hal_context.h#L21) est opaque pour le noyau et
  [`hal_contextStart()`](../../srcs/interfaces/hal_context.h#L31) enferme déjà le premier démarrage
  dans le HAL.
- Le scheduler appelle un timer par l'interface neutre `drv_timerSched.h` ; sa politique
  [round-robin](../../srcs/system/sysCore/sys_scheduler.c#L96) ne manipule aucun registre AVR.
- Les GPIO, USART, I2C et timers ont des contrats neutres. Les pilotes LCD et RTC peuvent donc être
  conservés si le nouveau backend I2C respecte exactement leur sémantique.
- La sélection `target -> board -> mcu -> arch` et les listes de sources autoCode permettent
  d'ajouter des implémentations sans `#if` matériel dans le code portable.
- Les piles, contextes et enregistrements de threads sont statiques : aucun allocateur dynamique
  n'est requis sur STM32. autoCode reste un outil hôte, il n'a pas à être porté sur le MCU.

## Blocages communs prioritaires

| Priorité | Blocage observé | Conséquence | Solution proposée |
|---|---|---|---|
| P0 | [`hal_threadContextInit()`](../../srcs/interfaces/hal_context.h#L28) ne reçoit que le sommet de pile | `makecontext()` exige base et taille ; aucune validation d'alignement ou de capacité | Passer base et taille en octets, ou un descripteur neutre, et générer cet appel par autoCode |
| P0 | [Taille unique de 256](../../srcs/interfaces/tm_modules.h#L56) `hal_stack_word_t` | 256 octets AVR, mais sens et coût différents sur amd64/ARM | Déplacer la taille vers le target ; exprimer la capacité en octets et imposer l'alignement par l'architecture |
| P0 | [Affectation directe](../../srcs/system/sysCore/sys_scheduler.c#L102) du contexte | La copie d'un `ucontext_t` peut conserver des pointeurs internes ; elle fige aussi la sémantique HAL | Ajouter une primitive HAL de capture/copie, ou faire passer au callback l'emplacement permanent du contexte courant |
| P0 | [`tm_vsnprintf()`](../../srcs/tmLibc/stdio/tm_snprintf.c#L176) lit `%i` avec `va_arg(args, uint16_t)` | Les promotions variadiques fournissent un `int`; l'hypothèse AVR 16 bits devient incorrecte sur ILP32 et LP64 | Lire le type promu (`int`/`unsigned int`), borner explicitement, et ajouter des tests 32/64 bits |
| P1 | `hal_context_t` et `hal_stack_word_t` complets sont injectés par `-include` | Contrat opaque en source mais taille requise dans `mod_thread_item_t`; chaque toolchain dépend d'un include forcé | Conserver un header de types sélectionné par architecture, mais tester sa présence, son alignement et sa cohérence avant autoCode |
| P1 | Le build final est spécifique AVR (`.elf`, `.hex`, `avr-size`, upload) | Ni exécutable hôte ni image ARM ne peuvent suivre les règles actuelles | Fournir un fragment de compilation/liaison par architecture et des cibles mémoire, image et flash propres |
| P1 | [L'USART](../../srcs/system/TaskMate.c#L38) est requise avant le scheduler, puis I2C/RTC/LCD au démarrage des run levels | Un backend minimal ne démarre pas l'application actuelle | Fournir ces capacités, ou créer un target de test minimal avec son propre `init.rc`, sans faux pilote dans le noyau |
| P1 | Deux sources temporelles sont supposées : quantum 1 ms et STC 10 ms | Un seul tick mal multiplexé dérive ou change l'ordre des callbacks | Soit deux timers, soit une base monotone unique et un diviseur documenté avec rattrapage des expirations |
| P2 | Des tailles et compteurs publics restent limités à 8 bits | Pas bloquant aujourd'hui, mais les limites silencieuses changent sur 32/64 bits | Conserver les bornes embarquées, ajouter des assertions et ne pas les remplacer aveuglément par `size_t` |

Ces changements sont système-critiques : les sources autoCode, les régions générées, les tests
du générateur et le build AVR doivent être modifiés et validés ensemble.

## Option A — simulation amd64/FreeBSD avec `ucontext`

### Architecture proposée

```text
srcs/hal/arch/amd64/          contexte, signaux, atomiques, arrêt, build natif
srcs/hal/mcu/freebsd/         timers et périphériques virtuels
srcs/hal/board/hostSim/       initialisation du processus et terminal
srcs/user/target/simFreeBSD/  init.rc, GPIO virtuels et choix applicatifs
```

Le nom `mcu/freebsd` est une adaptation de la taxonomie matérielle actuelle : il représente les
services de l'OS hôte et non un microcontrôleur. Une future famille de simulateurs pourrait justifier
une couche `platform`, mais elle n'est pas nécessaire pour ce premier port.

### Contextes et ordonnancement

`getcontext()` initialise chaque `ucontext_t`, puis `makecontext()` lui associe le point d'entrée,
`uc_stack.ss_sp`, `uc_stack.ss_size` et un lien de fin. `setcontext()` convient au premier démarrage.
Le retour d'une tâche doit aller vers un trampoline fatal explicite : une tâche TaskMate n'est pas
censée retourner.

Deux niveaux sont possibles :

1. **MVP coopératif** : `hal_timerSchedLoad()` effectue un basculement contrôlé. C'est simple et
   utile pour tester autoCode, les run levels et les syscalls, mais cela ne valide pas la préemption.
2. **Simulation préemptive** : un timer monotone délivre un signal installé avec `SA_SIGINFO` ; le
   handler reçoit le contexte interrompu, appelle la politique, puis laisse le retour de signal
   restaurer le contexte choisi.

Il ne faut pas supposer que `swapcontext()` est async-signal-safe. Le handler doit être minimal,
n'appeler ni stdio ni allocation, et la conception exacte de la modification du contexte de signal
doit être validée sur la version FreeBSD retenue. Le signal doit être bloqué durant l'initialisation,
la sélection et toute mise à jour non atomique partagée.

### Blocages FreeBSD et solutions

| Blocage | Solution envisageable | Validation requise |
|---|---|---|
| `ucontext` n'est plus dans POSIX.1-2008 | Assumer explicitement FreeBSD comme plateforme, isoler tous les appels dans `arch/amd64` | Compiler sur les versions FreeBSD supportées |
| Piles hôte trop petites | Taille target de plusieurs dizaines de Kio, page de garde facultative avec `mmap`/`mprotect` | Profondeur maximale, signaux imbriqués, débordement intentionnel |
| Copie et durée de vie de `ucontext_t` | Stockage permanent par thread et primitive HAL de capture ; ne pas exposer sa représentation au noyau | Registres, masque de signaux, FPU/SIMD sous optimisation |
| Tick perdu quand le processus est retardé | Timer monotone et lecture du nombre de dépassements ; rattraper les STC sans lancer une rafale non bornée | Charge CPU, pause debugger, dérive longue |
| Section atomique AVR = masque global | Retourner un token contenant l'ancien masque et bloquer seulement les signaux TaskMate | Imbrication, appel depuis handler, restauration exacte |
| `tm_stdio.h` choisit la libc selon le target | Décider si la simulation teste `tmLibc` ou la libc ; ne pas mélanger les deux dans un même oracle | Tests de formatage communs AVR/hôte |
| Périphériques absents | USART sur terminal non bloquant ; GPIO en état traçable ; bus I2C virtuel avec RTC/LCD, ou target minimal | Scénarios scriptés et erreurs injectées |
| Ordonnancement hôte non déterministe | Mode horloge virtuelle sans signaux pour CI, plus une suite préemptive séparée | Répétabilité CI et tests de stress |

### Ce que la simulation prouve et ne prouve pas

Elle peut valider la politique round-robin, les run levels, les syscalls, les STC, les services, les
tâches, SCLI et les erreurs de périphériques virtuels. Elle ne prouve ni l'empreinte SRAM/flash, ni
les latences et priorités d'IRQ, ni les registres volatils, ni la séparation Harvard/`PROGMEM`, ni le
comportement électrique, ni les garanties temps réel d'un MCU.

## Option B — portage STM32G4 sur carte Nucleo

### Cible de référence à figer

`STM32G4` et `Nucleo` ne désignent pas une cible unique. Le premier jalon doit figer la référence du
MCU, de la carte et sa révision. La **NUCLEO-G474RE** est un candidat cohérent : STM32G474RE,
Cortex-M4F, connecteur ST-LINK et format Nucleo-64. Les broches USART, I2C, LED, bouton, quartz et
ponts de soudure doivent provenir du manuel utilisateur de cette carte, pas d'hypothèses Arduino.

Organisation proposée :

```text
srcs/hal/arch/armv7em/          exceptions, contexte, atomiques, arrêt, toolchain
srcs/hal/mcu/stm32g474/         horloges, NVIC, TIM/SysTick, GPIO, USART, I2C
srcs/hal/board/nucleoG474RE/    startup carte, horloge et brochage ST-LINK/VCP
srcs/user/target/nucleoG474RE/  init.rc et signaux logiques
```

La couche architecture doit rester `armv7em`, et non `stm32`, afin que la mécanique Cortex-M puisse
servir à d'autres MCU. Les registres et vecteurs STM32 restent dans `mcu`; le brochage reste dans
`board`/`target`.

### Changement de contexte Cortex-M4F

Le chemin recommandé utilise PSP pour les threads, MSP pour les handlers, SysTick ou un TIM pour le
tick, et PendSV à la priorité la plus basse pour le changement effectif :

1. l'exception empile automatiquement R0-R3, R12, LR, PC et xPSR ;
2. PendSV sauvegarde R4-R11 et le PSP dans le contexte courant ;
3. le callback portable choisit le prochain contexte ;
4. PendSV restaure R4-R11, PSP et retourne d'exception ;
5. `hal_contextStart()` charge PSP/CONTROL et effectue un retour d'exception synthétique.

La pile initiale doit respecter l'alignement AAPCS de 8 octets, fournir xPSR avec le bit Thumb, un PC
pairé correctement et un LR vers le trampoline fatal. Si la FPU est autorisée, le contrat doit
préciser sauvegarde paresseuse et registres S16-S31 ; pour un premier jalon, la désactiver dans le
firmware TaskMate réduit fortement le risque. Des barrières `DSB`/`ISB` sont à placer selon le manuel
ARM lors des changements de masques, de priorités et de contexte.

### Blocages STM32G4 et solutions

| Blocage | Solution envisageable | Validation requise |
|---|---|---|
| Aucun startup ARM, table de vecteurs ou linker script | Ajouter une unité de startup MCU, sections flash/RAM, copie `.data`, zéro de `.bss`, symboles de pile et `Reset_Handler` | Map file, reset debugger et démarrage autonome |
| Sauvegarde AVR monolithique dans l'ISR timer | Séparer source du tick et PendSV ; garder la décision round-robin dans `sysCore` | Registres sentinelles, préemption à chaque instruction sensible |
| FPU et empilement conditionnel | FPU interdite au MVP, ou contexte étendu piloté par EXC_RETURN/FPCA | Tâches calcul flottant, IRQ, lazy stacking |
| Atomiques fondées sur SREG | Implémenter le token avec PRIMASK, ou BASEPRI si seules les IRQ TaskMate sont masquées ; restaurer exactement l'état entrant | Imbrication et appel depuis ISR |
| Horloge et ticks codés pour 16 MHz | Calculer les diviseurs depuis la fréquence effective, avec assertions d'erreur et unités explicites | Mesure GPIO/logic analyser sur 1 ms et 10 ms |
| Pilotes uniquement ATmega2560 | Implémenter GPIO, USART et I2C STM32 sous les contrats existants ; conserver LCD/RTC au-dessus | NACK, timeout, bus bloqué, RX/TX concurrents |
| `-fshort-enums` AVR et objets partagés avec ISR | Ne pas dépendre de la taille native des enums ; utiliser un stockage fixe et des conversions explicites | `sizeof`, warnings de conversion et optimisation LTO |
| Piles fixes non mesurées | Taille par target/thread, remplissage sentinelle et mesure high-water mark ; conserver des canaris | Charge maximale et IRQ imbriquées |
| Build AVR et upload `avrdude` | `arm-none-eabi-gcc`, options CPU/Thumb, linker script, `objcopy`, `size`; flash via STM32CubeProgrammer ou OpenOCD | Build propre, flash, reset et debug SWD |
| Dépendance possible aux bibliothèques constructeur | Choisir explicitement CMSIS seul ou LL ; enfermer les headers STM32 dans HAL et figer versions/options | Contrôle des includes et reproductibilité |
| Nucleo en 3,3 V face aux modules existants | Pull-up I2C à 3,3 V, vérification des niveaux et de l'alimentation, adaptation si nécessaire | Mesures sur carte et datasheets des modules |

### Choix des timers

Deux conceptions conviennent :

- SysTick à 1 ms demande PendSV, et un diviseur appelle le STC toutes les 10 occurrences ;
- un TIM produit le quantum et un second TIM produit le STC, au prix d'un périphérique de plus.

La première minimise le code et garantit une base commune. La seconde isole mieux les deux
contrats et ressemble davantage à l'AVR. Dans les deux cas, le changement de contexte doit rester
dans PendSV et l'ordre de traitement simultané doit être documenté.

## Comparaison et stratégie recommandée

| Critère | FreeBSD/amd64 | STM32G4/Nucleo |
|---|---|---|
| Valeur principale | CI fonctionnelle, diagnostic, injection d'erreurs | Portabilité MCU et comportement temps réel |
| Effort initial | Moyen | Élevé |
| Risque dominant | Signaux et sémantique `ucontext` | Exceptions, ABI, FPU, startup et horloges |
| Déterminisme | Fort en horloge virtuelle, faible en temps réel hôte | Fort si IRQ/priorités correctement conçues |
| Validation matérielle | Aucune | Obligatoire sur la référence exacte |
| Réutilisation | Tests rapides de toutes les couches hautes | Socle ARMv7E-M pour d'autres MCU |

Ordre conseillé :

1. ajouter des tests hôte du formatage variadique et corriger l'hypothèse `int == uint16_t` ;
2. faire évoluer pile/contexte/autoCode, puis prouver la non-régression AVR ;
3. livrer un target FreeBSD coopératif à horloge virtuelle ;
4. ajouter signaux et préemption FreeBSD dans une suite de stress distincte ;
5. figer NUCLEO-G474RE et livrer startup, linker, USART et GPIO sans scheduler ;
6. ajouter PSP/PendSV et un test minimal de deux threads sans FPU ;
7. ajouter STC, I2C, RTC/LCD, mesures temporelles et tests de débordement de pile ;
8. n'activer la FPU qu'après une décision explicite sur le contexte flottant.

## Critères de sortie

### Tronc commun

- build AVR inchangé fonctionnel, autoCode stable et aucun include de couche interdit ;
- tests du contrat de pile : base, taille, alignement, canaris et retour de tâche fatal ;
- tests de formatage sur ABI 16, 32 et 64 bits ;
- absence de référence au nouveau matériel hors de son target/HAL et du build sélecteur.

### FreeBSD

- scénario coopératif déterministe reproductible en CI ;
- préemption sous charge, restauration des masques, STC sans dérive non bornée ;
- exécution sous ASan/UBSan du mode compatible, plus tests d'arrêt et de pile ;
- documentation explicite des versions FreeBSD prises en charge.

### STM32G4

- build reproductible, budget flash/RAM et map file contrôlés ;
- registres généraux préservés à travers préemption et yield ;
- ticks 1 ms/10 ms mesurés, priorité PendSV vérifiée, atomiques testées avec IRQ imbriquées ;
- démarrage à froid, USART via ST-LINK, GPIO, I2C, RTC et LCD validés sur carte ;
- marge de pile mesurée pour chaque thread et comportement de faute documenté.

## Sources externes de référence

- [FreeBSD `getcontext(3)` / `makecontext(3)`](https://man.freebsd.org/cgi/man.cgi?query=getcontext&sektion=3)
- [FreeBSD `sigaction(2)`](https://man.freebsd.org/cgi/man.cgi?query=sigaction&sektion=2)
- [FreeBSD timers POSIX](https://man.freebsd.org/cgi/man.cgi?query=timer_create&sektion=2)
- [STMicroelectronics NUCLEO-G474RE](https://www.st.com/en/evaluation-tools/nucleo-g474re.html)
- [STMicroelectronics STM32G474RE](https://www.st.com/en/microcontrollers-microprocessors/stm32g474re.html)
- [ST RM0440, manuel de référence STM32G4](https://www.st.com/resource/en/reference_manual/rm0440-stm32g4-series-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)
- [Arm CMSIS-Core, Cortex-M4](https://arm-software.github.io/CMSIS_6/latest/Core/group__CMSIS__Core.html)

Les numéros de broches, alternate functions, fréquences maximales et révisions doivent être
revérifiés dans la datasheet du MCU et le manuel utilisateur de la révision de carte retenue au
moment de l'implémentation.
