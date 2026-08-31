# Audit des chemins entre services et HAL

## Périmètre

Cet audit analyse les dépendances entre `srcs/system/services/` et le HAL à la révision
`365a074`, pour la cible actuelle `test1 / arduinoMega / atmega2560 / avr8`.

Il distingue :

- les appels et inclusions HAL écrits directement dans les services ;
- les chemins transitifs qui atteignent le HAL sans passer par `sysCall` ;
- les chemins correctement médiés par `sysCall`.

L'analyse couvre les deux services enregistrés par autoCode, `msg` et `scli`, ainsi que leurs
commandes. Aucun chemin inverse du HAL vers les services n'a été trouvé.

## Mise à jour du 31 août 2026

Ce rapport reste un instantané historique de la révision `365a074`. Dans le code courant :

- `services_init.rc` enregistre `system` et `scli`; le service `msg` et ses accès LCD/USART ont été
  retirés ;
- le commit `5109e98` remplace les appels USART directs de SCLI par `sc_usartRead()` ;
- aucun fichier C ou en-tête de `srcs/system/services/` n'inclut un en-tête HAL ou n'appelle une
  fonction `hal_*` ;
- le chemin RX courant est `scliRead -> sc_usartRead -> hal_usartRead`, avec propagation des erreurs
  du driver par le syscall.

Le pont direct `services -> HAL` décrit ci-dessous n'est donc plus utilisé. Les chemins transversaux
de `tm_libc` vers son backend matériel existent toujours par conception et ne constituent pas un
accès HAL écrit directement dans un service.

## Conclusion de l'audit historique

Le code contient quatre chemins d'exécution directs `services -> HAL`, représentant huit sites
d'appel HAL dans deux unités de compilation :

| Service | Destination | Fonctions HAL | Sites d'appel |
|---|---|---|---:|
| `msg` | LCD | `hal_lcdSetCursor()`, `hal_lcdWriteString()` | 2 |
| `msg` | USART, message normal | `hal_usartWriteString()`, `hal_usartSendTXBuffer()` | 2 |
| `msg` | USART, destination inconnue | `hal_usartWriteString()`, `hal_usartSendTXBuffer()` | 2 |
| `scli` | USART RX | `hal_usartTestBufferRx()`, `hal_usartRead()` | 2 |

Ces accès contournent la direction prévue :

```text
services / tasks -> sysCall -> HAL -> interfaces
```

Ils couplent les services aux périphériques et à la cible matérielle courante. En outre, les quatre
implémentations C de services dépendent transitivement du HAL par `tm_string.h`.

## Chemins directs

### `msg -> LCD`

`msg.c` inclut directement la façade publique LCD
([msg.c](https://github.com/spradere/TaskMate/blob/365a074ce5ed1c2029e7e6a44adcfaa0540ca1ca/srcs/system/services/msg.c#L22)). Dans la branche `MSG_TO_LCD`, `msgProcess()`
appelle successivement :

```text
msgProcess
  -> hal_lcdSetCursor
  -> hal_lcdWriteString
```

Les appels se trouvent dans
[`msgProcess()`](https://github.com/spradere/TaskMate/blob/365a074ce5ed1c2029e7e6a44adcfaa0540ca1ca/srcs/system/services/msg.c#L106), aux lignes 117 et 126.

La façade sélectionne concrètement le pilote AMC2004 uniquement pour `HWT_test1`
([lcd.h](../../srcs/hal/public/lcd.h#L18)). Le service ne peut donc pas être compilé tel quel pour
une cible sans ce LCD.

### `msg -> USART`

`msg.c` inclut aussi directement la façade USART
([msg.c](https://github.com/spradere/TaskMate/blob/365a074ce5ed1c2029e7e6a44adcfaa0540ca1ca/srcs/system/services/msg.c#L23)). Deux branches atteignent le pilote :

```text
MSG_TO_USART
  -> hal_usartWriteString
  -> hal_usartSendTXBuffer

destination inconnue
  -> hal_usartWriteString
  -> hal_usartSendTXBuffer
```

Les appels sont aux lignes 131-132 et 140-141 de
[`msgProcess()`](https://github.com/spradere/TaskMate/blob/365a074ce5ed1c2029e7e6a44adcfaa0540ca1ca/srcs/system/services/msg.c#L106).

La façade USART sélectionne directement l'implémentation `atmega2560`
([usart.h](../../srcs/hal/public/usart.h#L18)). Cette dépendance empêche de réutiliser `msg` sur un
autre MCU sans fournir la même capacité ou modifier le service.

### `scli -> USART RX`

`scli.c` inclut directement `hal/public/usart.h`
([scli.c](https://github.com/spradere/TaskMate/blob/365a074ce5ed1c2029e7e6a44adcfaa0540ca1ca/srcs/system/services/scli.c#L20)). Le chemin de lecture est :

```text
scliRead
  -> hal_usartTestBufferRx
  -> hal_usartRead
```

Il se trouve dans [`scliRead()`](https://github.com/spradere/TaskMate/blob/365a074ce5ed1c2029e7e6a44adcfaa0540ca1ca/srcs/system/services/scli.c#L63), aux lignes 67 et 71. Le
service dépend ainsi du buffer et des codes d'erreur propres au pilote USART actuel.

## Défauts observés sur ces chemins

### P1 - Résultats HAL ignorés par `msg`

`msgProcess()` ignore les résultats de ses six appels HAL :

- la branche USART efface `MSG_FLAG_SEND` même si l'écriture ou l'envoi échoue ;
- la branche LCD n'efface jamais `MSG_FLAG_SEND` et retransmet périodiquement le même message ;
- la branche de destination inconnue ne définit pas de politique explicite de rejet ou de nouvelle
  tentative.

Les fonctions publiques LCD et USART refusent pourtant les opérations lorsque leur driver n'est pas
`RUNNING` ([lcd_AMC2004.c](../../srcs/hal/drivers/lcd_AMC2004/lcd_AMC2004.c#L111),
[usart.c](../../srcs/hal/mcu/atmega2560/usart.c#L108)). Ces erreurs restent donc invisibles au
service.

### P1 - Index de ligne LCD non validé

Le premier octet du message devient directement l'argument `row` de `hal_lcdSetCursor()`
([msg.c](https://github.com/spradere/TaskMate/blob/365a074ce5ed1c2029e7e6a44adcfaa0540ca1ca/srcs/system/services/msg.c#L117)). Le pilote utilise ensuite cet argument pour
indexer un tableau de quatre offsets sans vérifier sa borne
([lcd_AMC2004.c](../../srcs/hal/drivers/lcd_AMC2004/lcd_AMC2004.c#L111)).

Un message LCD dont le premier octet est supérieur à trois peut donc provoquer une lecture hors
limites dans le HAL.

### P1 - État USART mal interprété par `scli`

`scliRead()` considère toute valeur différente de `ERR_HAL_USART_RX_BUFFER_EMPTY` comme indiquant
des données disponibles. Or `hal_usartTestBufferRx()` renvoie également `ERR_RUNTIME` lorsque le
driver n'est pas `RUNNING` ([usart.c](../../srcs/hal/mcu/atmega2560/usart.c#L150)).

Le service entre alors dans la branche de lecture, mais `hal_usartRead()` échoue immédiatement. Le
contrat correct doit distinguer explicitement `ERR_NO_ERROR` des autres états.

### P2 - Sortie USART synchrone dans le service

`hal_usartSendTXBuffer()` attend activement que le registre USART puisse accepter chaque octet
([usart.c](../../srcs/hal/mcu/atmega2560/usart.c#L136)). Appelée directement par `msg`, cette
attente bloque le thread de service pendant toute l'émission et expose le service aux détails
temporels du pilote.

## Chemins transitifs sans médiation syscall

Les quatre fichiers C de services incluent `tm_string.h`. Avec la libc TaskMate sélectionnée, cet
en-tête inclut `hal/public/tmlibc.h` pour obtenir les macros de stockage AVR
([tm_string.h](../../srcs/tm_libc/tm_string.h#L36)). Toutes les implémentations de services ont donc
une dépendance de compilation transitive au HAL.

Les principaux chemins d'exécution associés sont :

```text
services -> tm_strncmp/tm_strncpy -> hal_string_getChar

services -> tm_syslog -> tm_vprintf -> hal_stdio_putChar -> USART
```

`tm_string.c` utilise `hal_string_getChar()` pour chaque comparaison ou copie
([tm_string.c](../../srcs/tm_libc/tm_string.c#L17)). Les diagnostics des commandes et de `scli`
passent par `tm_syslog()`, puis par la sortie HAL de `tm_libc`
([tm_snprintf.c](../../srcs/tm_libc/tm_snprintf.c#L85),
[tmlibc.c](../../srcs/hal/mcu/atmega2560/tmlibc.c#L19)).

Ces appels ne sont pas écrits directement sous la forme `hal_*` dans les services, mais ils
contournent eux aussi `sysCall`.

## Chemins correctement médiés

La commande SCLI `driver` n'accède pas directement aux drivers. Elle utilise :

```text
service command/driver
  -> sc_driverGetInfo / sc_driverInit / sc_driverStart / sc_driverStop
  -> mod_driver_item_t.control
  -> hal_<driver>Control
```

Les appels du service sont dans
[`commands/driver.c`](../../srcs/system/services/commands/driver.c#L81). La médiation est
implémentée dans [`sysCall.c`](../../srcs/system/sysCall/sysCall.c#L93), et les callbacks vers les
six drivers sont attribués dans la table générée
([modules.c](../../srcs/system/sysCore/modules.c#L133)). Ce chemin peut atteindre LCD, RTC,
`timerSched`, `timerSTC`, I2C et USART, mais il respecte la frontière syscall.

Les opérations de threads et les délais logiciels utilisés par les services passent également par
les fonctions `sc_thread*` et `sc_coopYield()` ; ils ne constituent pas des accès HAL directs depuis
les services.

## Limite du contrôle de frontières

Le contrôle de build analyse uniquement les motifs déclarés dans
[`conf/header_allow.conf`](../../conf/header_allow.conf#L15). Les façades `hal/public/lcd.h`,
`hal/public/usart.h` et `hal/public/tmlibc.h` n'y sont pas déclarées. Les dépendances identifiées
dans cet audit sont donc acceptées silencieusement par la vérification actuelle.

Les gardes de compilation protègent quelques interfaces système critiques, mais elles ne définissent
pas une politique générale interdisant les inclusions HAL depuis `srcs/system/services/`.

## Recommandations

État actuel : la médiation syscall de la lecture série et le retrait des inclusions HAL de `msg` et
`scli` sont réalisés. Les recommandations restantes sont conservées comme historique ou travaux de
durcissement, notamment l'interdiction explicite des en-têtes HAL dans la politique de frontières.

1. Introduire des contrats syscall pour la lecture série et les sorties de messages, avec des codes
   de résultat explicites.
2. Retirer les inclusions `hal/public/lcd.h` et `hal/public/usart.h` de `msg.c` et `scli.c`.
3. Séparer le contrat portable des chaînes de la sélection matérielle de leur stockage, afin que
   `tm_string.h` n'expose plus directement `hal/public/tmlibc.h` aux services.
4. Définir une politique de message en cas d'échec : nouvelle tentative bornée, abandon explicite ou
   compteur de perte.
5. Étendre le contrôle de build avec une règle de frontière interdisant les en-têtes HAL dans les
   services, y compris par inclusion transitive.
