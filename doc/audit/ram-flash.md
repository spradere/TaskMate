# Audit RAM et flash

## Périmètre

Cet audit analyse le firmware courant pour la cible
`test1 / arduinoMega / atmega2560 / avr8` à la révision `39907b4`.
Il couvre les données statiques, les constantes, les chaînes, les tables générées, les buffers,
le code effectivement lié et les options de compilation.

Les tableaux de piles des threads sont volontairement exclus. Aucun redimensionnement de pile
n'est proposé. Les autres champs d'un thread (état, compteur logiciel, pointeur de pile, canaris,
fonction et éventuel nom) restent comptés, car ils devront encore exister avec des piles de taille
dynamique.

L'analyse repose sur :

- une compilation BSD `bmake` complète dans une copie sous `/tmp`, afin de ne modifier aucun
  fichier généré du dépôt ;
- `avr-size`, `avr-nm`, la map de l'éditeur de liens et le contenu des sections ELF ;
- la vérification des définitions et de leurs appels ;
- deux variantes temporaires compilées pour mesurer des gains flash précis.

## Mesure de référence

La compilation courante produit :

| Mesure | Valeur |
|---|---:|
| `.text` | 8 310 octets |
| `.data` | 414 octets |
| `.bss` | 1 543 octets |
| Flash, `.text + .data` | **8 724 / 65 536 octets, 13,31 %** |
| SRAM annoncée, `.data + .bss` | 1 957 / 8 192 octets |
| Piles de threads exclues | -1 024 octets |
| SRAM analysée hors piles | **933 / 8 192 octets, 11,39 %** |

La limite flash de 64 Kio est celle retenue par le port AVR actuel, même si l'ATmega2560 physique
dispose de davantage de flash
([mcu_make.mk](../../srcs/hal/mcu/atmega2560/mcu_make.mk#L30)).

La section `.data` coûte deux fois : 414 octets de SRAM après le démarrage et 414 octets de flash
pour son image initiale.

## Répartition exacte de la SRAM hors piles

| Poste | SRAM | Origine |
|---|---:|---|
| Littéraux C ordinaires recopiés en RAM | 266 octets | messages de tâches/services, panic, canaris et chiffres de conversion |
| Canaux de messages | 260 octets | 4 × (1 octet d'état + 64 octets de texte) |
| USART | 132 octets | RX 64, TX 64 et quatre indices |
| Table des ports MCU | 72 octets | 12 ports × 3 pointeurs de 16 bits |
| Métadonnées des threads hors piles | 68 octets | 52 octets dans les 4 structures + 16 octets de descripteurs de noms |
| Métadonnées générées des drivers | 72 octets | table runtime 24, descripteurs de noms 24, image d'initialisation 24 |
| Table des trois signaux GPIO | 24 octets | 3 × `hal_signal_t` de 8 octets |
| Buffer RTC | 7 octets | buffer global des sept registres |
| État global de `tm_snprintf` | 6 octets | verrou et descripteur de sortie |
| États des six drivers | 6 octets | un octet par driver |
| Callbacks des deux timers | 4 octets | deux pointeurs de fonction |
| Identifiants des canaux des tâches/services | 3 octets | task1, task2 et scli |
| Thread courant | 1 octet | base des modules |
| Descripteurs de chaînes version/fichier | 8 octets | deux `tm_string_t` |
| Offsets de lignes LCD | 4 octets | tableau local `const` placé en `.data` |
| **Total hors piles** | **933 octets** | |

Les quatre états de canaux et leurs textes sont alloués dans
[`msg.c`](../../srcs/system/services/msg.c#L29). Les buffers USART sont alloués dans
[`usart.c`](../../srcs/hal/mcu/atmega2560/usart.c#L23). La base des modules est définie dans
[`modules.c`](../../srcs/system/sysCore/modules.c#L24).

## Recommandations prioritaires

### P0 — Activer la relaxation AVR : gain mesuré de 550 octets de flash

Le port utilise déjà les bons fondamentaux : `-Os`, LTO, sections séparées et
`--gc-sections`
([arch_make.mk](../../srcs/hal/arch/avr8/arch_make.mk#L34)).
Il manque `-mrelax`.

Une compilation temporaire avec cette option passe intégralement et donne :

| Configuration | Flash | SRAM |
|---|---:|---:|
| Référence | 8 724 octets | 1 957 octets |
| Avec `-mrelax` | 8 174 octets | 1 957 octets |
| Gain | **550 octets** | 0 octet |

La relaxation élimine ici 264 octets de trampolines et raccourcit d'autres appels/sauts.
L'option doit rester dans le fichier de l'architecture AVR, pas dans les options portables.
Comme elle modifie les appels, les vecteurs et le placement final, il faut valider sur carte le boot,
les ISR, le changement de contexte et le scheduler avant adoption.

### P0 — Désactiver les diagnostics de disposition en production : gain mesuré de 680 octets

`mod_threadsAlloc()` imprime toujours neuf informations de disposition de structure
([modules.c](../../srcs/system/sysCore/modules.c#L50)), même avec `VERBOSE_LEVEL == 0`
([options.h](../../srcs/interfaces/options.h#L18)).

Les neuf formats occupent exactement **450 octets de flash**. Une variante temporaire qui retire
uniquement ces appels, compilée avec `-mrelax`, passe de 8 174 à 7 494 octets :
**680 octets de flash gagnés**, sans changement SRAM.

Il est préférable de conserver ce diagnostic derrière une option de debug éliminée à la
précompilation. Un test runtime ne permettrait pas à l'éditeur de liens de retirer les chaînes et le
code.

### P1 — Rendre les API de texte conscientes de la mémoire : jusqu'à 265 octets de SRAM

Le support `TM_STR` place correctement ses caractères en flash sur AVR
([tmlibc.h](../../srcs/hal/mcu/atmega2560/tmlibc.h#L26)). En revanche,
`msgWriteText()`, `hal_usartWriteString()`, `hal_lcdWriteString()` et `panic()` prennent un
`const char *` RAM. Les littéraux qui leur sont passés sont donc copiés dans `.data`.

Le binaire contient **265 octets de caractères constants**, plus un octet d'alignement, dans cette
situation. Les principaux appels sont dans
[`task1.c`](../../srcs/user/tasks/task1.c#L27),
[`task2.c`](../../srcs/user/tasks/task2.c#L27),
[`msg.c`](../../srcs/system/services/msg.c#L54),
[`scli.c`](../../srcs/system/services/scli.c#L28),
[`panic.c`](../../srcs/hal/arch/avr8/panic.c#L20) et
[`tm_scheduler.c`](../../srcs/system/sysCore/tm_scheduler.c#L60).

La bonne correction n'est pas de convertir un pointeur `PROGMEM` en `char *`. Sur AVR Harvard,
ce pointeur doit être lu avec l'accès flash approprié. Il faut faire accepter `tm_string_t` aux API
concernées, ou fournir des variantes RAM/ROM explicites, puis laisser les buffers produits à
l'exécution utiliser `TM_STR_RAM`.

Le tableau local `"0123456789abcdef"` de
[`tm_snprintf.c`](../../srcs/tm_libc/tm_snprintf.c#L39) représente à lui seul 17 octets de SRAM.
Un calcul `0..9 / a..f` évite ce tableau sans lecture `PROGMEM`.

Déplacer les littéraux vers la flash économise la SRAM mais pas leurs caractères en flash. Supprimer
les messages de test inutiles économise les deux.

### P1 — Générer directement la séquence de démarrage : 72 octets de SRAM évitables

Dans le firmware actif, la table des drivers n'est utilisée que pendant l'allocation et la double
boucle de boot
([boot.c](../../srcs/system/sysCore/boot.c#L48)). Les autres parcours de drivers dans
`TaskMate.c` sont commentés.

Pour six drivers, l'implémentation coûte :

- 24 octets pour `mod_driver_item_t drivers[6]` ;
- 24 octets pour les six descripteurs `tm_string_t` des noms ;
- 24 octets supplémentaires en `.data` pour l'image constante des six affectations composées.

autoCode connaît déjà le run level, le nom et la fonction `hal_<driver>Control`
([parseTag.c](../../srcs/autoCode/parseTag.c#L370)). Il peut générer une séquence directe, ordonnée par
run level, avec des appels de fonction directs et des noms flash seulement quand les logs sont
activés. Cela supprime **72 octets de SRAM**, les 24 octets correspondants de l'image
d'initialisation en flash, les appels indirects et la recherche `run level × drivers`.

Cette modification doit être faite dans autoCode et ses entrées `*.rc`, jamais dans la zone
générée de `modules.c`. Il faut aussi rendre l'ordre déterministe : la collecte actuelle des
`*.rc` utilise un `find` non trié
([sources.mk](../mk/sources.mk#L37)).

### P1 — Supprimer le double buffering TX : 66 octets de SRAM

Le chemin message copie actuellement le texte dans un canal de 64 octets, puis le copie dans le ring
TX USART de 64 octets, puis vide ce ring de manière bloquante
([msg.c](../../srcs/system/services/msg.c#L97),
[usart.c](../../srcs/hal/mcu/atmega2560/usart.c#L116)).

Comme l'émission finale attend déjà `UDRE1`, un chemin d'écriture bloquant direct peut supprimer le
buffer TX et ses deux indices : **66 octets de SRAM**, avec potentiellement moins de code. Une autre
solution cohérente serait une vraie émission TX par interruption ; elle conserve alors un buffer,
mais change réellement la sémantique en asynchrone.

Le buffer RX est utilisé depuis une ISR et doit rester `volatile`. Sa taille ne doit être réduite
qu'après instrumentation des débordements.

### P1 — Dimensionner les canaux à partir du besoin : formule de gain explicite

`channels[4]` occupe **260 octets**, le premier poste de BSS hors piles
([msg.c](../../srcs/system/services/msg.c#L29)).

- passer de 64 à 32 caractères économise `4 × 32 = 128` octets ;
- retirer un canal économise 65 octets ;
- séparer le nombre de canaux et la longueur par cible permet de garder une politique déterministe.

Le choix doit être fondé sur le nombre maximal de producteurs simultanés et la longueur maximale
observée. Une conception zéro-copie avec pointeur emprunté n'est sûre que si la durée de vie et la
propriété du texte sont garanties entre producteurs et consommateur.

### P1 — Compacter ou supprimer la table complète des ports : jusqu'à 60 octets

`mcu_ports[PORT_COUNT]` réserve 72 octets en SRAM et autant dans l'image flash
([gpio.c](../../srcs/hal/mcu/atmega2560/gpio.c#L23)). Douze ports sont réservés, mais seuls A et B ont
une entrée et seuls ces deux ports sont utilisés par la cible actuelle
([targetInit.c](../../srcs/user/target/test1/targetInit.c#L20)).

Une table compacte des ports réellement sélectionnés économiserait ici jusqu'à **60 octets de SRAM
et 60 octets de flash**. Une autre option est un `switch` MCU qui matérialise directement les
adresses de registres. Il ne faut pas placer simplement la table en `PROGMEM` puis la déréférencer
comme de la RAM.

La sélection doit rester spécifique au MCU/à la cible dans l'arbre et le build, sans introduire des
`#if` matériels dans le code portable.

## Optimisations secondaires

### Noms des threads

Les noms des quatre threads ne sont pas lus par le firmware actif. Leurs descripteurs coûtent
16 octets de SRAM et les pointeurs `name` dans les structures coûtent 8 octets. Une option
`TM_DEBUG_NAMES` éliminée à la compilation permettrait donc d'économiser **24 octets de SRAM** et
21 octets de caractères en flash, plus un peu de code d'initialisation.

Il ne faut pas retirer les autres métadonnées du scheduler sous prétexte qu'elles sont voisines des
piles.

### Types enum des GPIO

Sur cet AVR, les enums occupent deux octets. `hal_pin_t` contient trois enums, puis
`hal_signal_t` ajoute un booléen
([mcu_define.h](../../srcs/hal/mcu/atmega2560/mcu_define.h#L26),
[gpio.h](../../srcs/hal/mcu/atmega2560/gpio.h#L22)). Remplacer les types de stockage par des
`uint8_t` explicites avec constantes nommées ferait passer les trois entrées de 24 à environ
15 octets, soit **9 octets estimés**.

Il vaut mieux modifier explicitement les contrats concernés que forcer globalement
`-fshort-enums`, qui change l'ABI de tous les enums.

Le champ `active_high` n'est actuellement pas appliqué par
[`gpio.c`](../../srcs/system/sysCore/gpio.c#L30), mais il exprime une exigence de portabilité. Il doit
être implémenté et compacté, pas supprimé uniquement pour gagner de la RAM.

### RTC et offsets LCD

Le buffer RTC global de sept octets
([rtc_ZS042.c](../../srcs/hal/drivers/rtc_ZS042/rtc_ZS042.c#L24)) peut être remplacé par un transfert en
flux avec un octet temporaire. Cela économise **7 octets statiques** sans changer le protocole I2C.
Il faut simultanément propager les erreurs I2C.

Le tableau `row_offsets` du LCD est un `const` local, mais occupe quatre octets de `.data`
([lcd_AMC2004.c](../../srcs/hal/drivers/lcd_AMC2004/lcd_AMC2004.c#L109)). Un petit `switch` ou une
lecture flash économise ces **4 octets de SRAM**. L'index `row` doit être validé.

### Descripteurs de chaînes

`tm_string_storage_t` est un enum de deux octets ; un `tm_string_t` occupe donc quatre octets
([tm_string_storage.h](../../srcs/interfaces/tm_string_storage.h#L20)). Un stockage explicite sur
`uint8_t` ramènerait normalement le descripteur à trois octets. Le gain est faible par instance,
mais devient intéressant si toutes les API texte utilisent ce type.

### Formateur et logs

`tm_vsnprintf` occupe **568 octets de flash** dans la référence, ou 538 avec `-mrelax`
([tm_snprintf.c](../../srcs/tm_libc/tm_snprintf.c#L104)). Les formats `%b` et `%c` ne sont pas
utilisés par le firmware actif ; `%x` ne sert qu'au scan I2C. Des options de fonctionnalités
compilées peuvent permettre à LTO d'éliminer les conversions inutilisées.

Le verrou et le descripteur global du formateur occupent six octets. Les déplacer sur une pile ne
constituerait pas une réduction réelle de RAM et n'est donc pas retenu comme recommandation.

### Dispatchs des drivers

Les six fonctions `hal_*Control()` et leurs tables de saut représentent environ **1 400 octets de
flash** dans la référence. Leur état d'un octet est déjà compact. Une fonction commune pour les
commandes d'état pourrait réduire la répétition, mais elle ajoute des pointeurs et des appels sur un
chemin HAL sensible. Ce refactoring ne doit être entrepris qu'avec une variante compilée et une
validation des cycles/ISR ; il est moins sûr que les gains P0/P1 précédents.

## Code de test et diagnostics à isoler

Trois blocs ne devraient pas être imposés à un firmware de production :

- la zone expérimentale RTC/LCD de
  [TaskMate.c](../../srcs/system/TaskMate.c#L48) ;
- le scan des 127 adresses dans le démarrage du driver I2C
  ([i2c.c](../../srcs/hal/mcu/atmega2560/i2c.c#L56)) ;
- les messages de démonstration de `msg`, `task1`, `task2` et `scli`.

Le scan I2C ajoute deux formats flash, une boucle et du code de formatage hexadécimal dans un driver
MCU générique. Il doit devenir un outil de diagnostic explicite, pas une étape normale de
`Start()`. La zone expérimentale doit rester sélectionnable par la cible `test1` ou une option de
build. Des gardes de compilation permettent à LTO et `--gc-sections` de supprimer totalement ces
chemins.

Le catalogue d'erreurs généré, bien que présent dans les sources, n'est pas lié dans le firmware
actuel parce que son seul appel est commenté et que `--gc-sections` fonctionne. Il n'est donc pas
un poste à optimiser dans la mesure actuelle.

## Ordre d'implémentation recommandé

1. Ajouter `-mrelax` au port AVR et valider boot, ISR, contexte et scheduler sur Arduino Mega.
2. Introduire un profil de production qui élimine les diagnostics de disposition, le scan I2C et
   les démonstrations de la cible.
3. Étendre le contrat de chaînes RAM/ROM, puis convertir tous les littéraux constants sans casser le
   modèle Harvard.
4. Générer une séquence directe de démarrage et supprimer la table runtime des drivers si aucune API
   de diagnostic ne l'exige.
5. Supprimer le buffer TX synchrone, puis dimensionner canaux et RX à partir de mesures
   d'overflow/longueur.
6. Compacter la représentation GPIO et la table des ports.
7. Appliquer les petits gains RTC, LCD, noms de threads et descripteurs de chaînes.

Après chaque étape : exécuter `bmake autoCode_alone` si la source de vérité est concernée, relire le
log et les zones générées, lancer un `bmake` complet, comparer `.text/.data/.bss` et la map, puis
faire les validations matérielles proportionnées au changement.

## Objectif réaliste

Sans toucher aux piles, les gains statiques les plus directs totalisent déjà :

- **550 octets de flash mesurés** avec `-mrelax` ;
- **680 octets de flash mesurés** en désactivant les diagnostics de disposition, en plus de
  `-mrelax` ;
- jusqu'à **265 octets de SRAM** en corrigeant le stockage des littéraux ;
- **72 octets de SRAM** en supprimant les métadonnées runtime des drivers ;
- **66 octets de SRAM** en supprimant le buffer TX synchrone ;
- jusqu'à **60 octets de SRAM** en compactant les ports pour la cible actuelle.

Ces gains ne sont pas tous strictement additifs : supprimer un bloc de test peut aussi supprimer ses
chaînes, et une séquence directe de boot peut rendre d'autres métadonnées mortes. Il faut donc mesurer
le cumul après chaque changement plutôt que sommer mécaniquement les maxima.
