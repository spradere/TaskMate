# Analyse de l'arborescence du projet

## Périmètre

Cette analyse examine l'organisation du dépôt à la révision `3035005`, en particulier :

- la séparation entre système, code utilisateur, HAL, interfaces et bibliothèque C ;
- la composition matérielle `HWT / board / MCU / architecture` ;
- la place d'autoCode et des fichiers générés ;
- la correspondance entre l'organisation physique et les dépendances réelles du code ;
- l'organisation du build, de la documentation et des scripts hôte.

Il s'agit d'une analyse statique sans modification du code et sans compilation du firmware. Deux
lectures du même diagnostic sont proposées : une destinée à un développeur junior et une destinée
à un développeur senior.

### Mise à jour du 31 août 2026

Les constats concernant les accès directs de `msg` et `scli` au HAL sont historiques. Le service
`msg` a été retiré et SCLI lit désormais l'USART par `sc_usartRead()`. Une recherche dans les sources
et en-têtes actuels de `srcs/system/services/` ne trouve plus d'inclusion HAL ni d'appel `hal_*`.
Les autres constats de cet audit restent rattachés à la révision `3035005`.

## Conclusion générale

L'arborescence constitue une base saine pour un RTOS embarqué expérimental. Elle rend visibles les
intentions principales de portabilité, de déterminisme et de séparation entre système et matériel.

La faiblesse principale n'est pas le nombre ou le nom des dossiers : les frontières annoncées par
l'arborescence ne correspondent pas encore complètement au graphe de dépendances réel. Une
réorganisation massive apporterait peu de valeur aujourd'hui. Il est préférable de corriger d'abord
les dépendances qui traversent les couches, puis de déplacer uniquement les responsabilités devenues
clairement autonomes.

## Lecture pour un développeur junior

### Organisation générale

Le projet est globalement bien rangé : chaque grand dossier répond à une question identifiable.

```text
srcs/
├── system/       cœur du RTOS, syscalls et services
├── user/         tâches et configuration de la cible
├── hal/          implémentation liée au matériel
├── interfaces/   types et contrats partagés
├── tm_libc/      petite bibliothèque adaptée au MCU
└── autoCode/     générateur exécuté sur la machine de développement
```

Les fichiers de build sont également séparés du firmware :

- `Makefile` orchestre le build ;
- `mk/` contient les fragments BSD `bmake` ;
- `conf/` contient les listes et politiques déclaratives ;
- `scripts/` contient les traitements shell et AWK ;
- `doc/` regroupe les règles, l'architecture et les audits.

Cette organisation aide à trouver l'emplacement naturel d'une modification sans parcourir tout le
dépôt.

### Partie matérielle

La subdivision du HAL est particulièrement utile :

```text
hal/
├── arch/avr8/          fonctionnement du CPU AVR
├── mcu/atmega2560/     registres et périphériques du MCU
├── board/arduinoMega/  caractéristiques de la carte
├── drivers/            composants LCD et RTC
└── public/             points d'entrée sélectionnés pour la cible
```

Ainsi, un changement de contexte appartient à
[`hal/arch/avr8/context.c`](../../srcs/hal/arch/avr8/context.c), tandis qu'un changement sur les
registres série appartient à
[`hal/mcu/atmega2560/usart.c`](../../srcs/hal/mcu/atmega2560/usart.c).

La cible `test1` assemble les différents niveaux par une chaîne explicite :

```text
user/target/test1
  -> board/arduinoMega
     -> mcu/atmega2560
        -> arch/avr8
```

Cette composition est visible dans [`hwt.mk`](../../srcs/user/target/test1/hwt.mk#L29),
[`board_make.mk`](../../srcs/hal/board/arduinoMega/board_make.mk#L22) et
[`mcu_make.mk`](../../srcs/hal/mcu/atmega2560/mcu_make.mk#L24).

### Point de vigilance

Le nom du dossier indique la responsabilité souhaitée, mais ne garantit pas à lui seul que toutes les
dépendances respectent cette responsabilité. À la révision auditée, le service de messages incluait
directement les façades LCD et USART. Ce fichier a depuis été retiré et ce chemin direct n'existe plus
dans les services actuels.

La règle à suivre pour un nouveau code reste :

```text
tasks / services -> sysCall -> HAL -> interfaces
```

Avant d'ajouter un fichier, il faut identifier le niveau propriétaire de l'information :

- politique du RTOS : `system/` ;
- demande de l'application : `user/` ;
- contrat portable : `interfaces/` ;
- registre ou particularité matérielle : `hal/` ;
- outil exécuté sur l'hôte : `autoCode/`, `scripts/` ou `mk/`.

Il n'est pas utile d'ajouter un nouveau dossier lorsqu'une responsabilité existante correspond déjà
clairement au changement.

## Lecture pour un développeur senior

### Appréciation d'ensemble

La macro-architecture est cohérente : séparation hôte/firmware, HAL stratifié, composition statique de
la cible et build modulaire. Pour la taille actuelle du projet, une restructuration générale
augmenterait surtout le bruit des diffs et le risque de régression.

L'enjeu est plutôt d'aligner le graphe de dépendances avec les frontières que l'arborescence promet.

### `hal/public` est un routeur, pas encore un contrat neutre

Les en-têtes de `hal/public` sélectionnent des implémentations concrètes par préprocesseur. Par exemple,
[`hal/public/usart.h`](../../srcs/hal/public/usart.h#L18) inclut directement l'en-tête USART de
l'ATmega2560, tandis que [`hal/public/lcd.h`](../../srcs/hal/public/lcd.h#L18) sélectionne le driver
AMC2004 pour `HWT_test1`.

Ce mécanisme est simple et peu coûteux avec une seule cible. Avec plusieurs MCU et plusieurs
capacités optionnelles, il tendra cependant vers une matrice de branches conditionnelles. À terme,
`hal/public` devrait exposer des contrats stables, la sélection de l'implémentation restant interne au
HAL ou pilotée par le build.

### Les dépendances réelles traversent encore les couches

À la révision auditée, plusieurs relations contredisaient la topologie souhaitée :

- `msg` et `scli` accédaient directement au HAL LCD/USART ; ce point est maintenant corrigé ;
- les drivers LCD et RTC incluent le contrôleur I2C concret de l'ATmega2560, par exemple
  [`lcd_AMC2004.c`](../../srcs/hal/drivers/lcd_AMC2004/lcd_AMC2004.c#L19) ;
- `tm_libc` dépend à la fois du HAL et de `sysCall`
  ([tm_snprintf.c](../../srcs/tm_libc/tm_snprintf.c#L17)) ;
- le panic AVR dépend du backend USART du MCU
  ([panic.c](../../srcs/hal/arch/avr8/panic.c#L18)).

Le placement physique est donc lisible, mais il ne fournit pas encore un découplage suffisant pour
remplacer indépendamment une couche.

### `interfaces/` mélange contrats portables et configuration kernel

Le dossier contient des types neutres comme les modes GPIO et le stockage des chaînes, mais aussi
des informations système générées et protégées comme
[`modules_define.h`](../../srcs/interfaces/modules_define.h).

Cette double responsabilité brouille la nature du dossier. Une direction possible serait :

```text
interfaces/        contrats portables réellement partagés
system/generated/  configuration et tables produites pour le kernel
```

Cette séparation ne doit toutefois être faite qu'avec une migration atomique des sources d'autoCode,
des régions générées, des consommateurs et des listes d'autorisation.

### Les fichiers générés sont mêlés aux sources manuelles

Le programme autoCode est clairement isolé sous `srcs/autoCode`, mais il génère des régions ou des
fichiers dans `system/`, `interfaces/` et `hal/public/`. Le build génère aussi
`interfaces/taskmate_info.h` depuis [`build.mk`](../../mk/build.mk#L24).

Un sous-arbre généré explicite améliorerait la lecture des diffs, les builds propres et la
reproductibilité. Sur AVR, cette évolution doit rester purement structurelle : elle ne doit pas ajouter
de couche runtime, d'allocation ou d'indirection.

### La portabilité de l'arborescence reste à démontrer

Une seule pile est implémentée : `test1 / arduinoMega / atmega2560 / avr8`. La meilleure validation
de l'organisation serait l'ajout d'une seconde cible, par exemple un simulateur hôte minimal ou un
autre MCU. Cette cible ferait apparaître rapidement :

- les contrats HAL réellement neutres ;
- les hypothèses AVR qui subsistent dans les couches communes ;
- les capacités matérielles obligatoires ou optionnelles ;
- les points où la composition par inclusions Make devient trop rigide.

### Priorités recommandées

1. Maintenir l'absence de dépendance directe `services -> HAL` et supprimer les dépendances
   `drivers -> MCU concret` restantes.
2. Faire de `hal/public` une couche de contrats plutôt qu'un ensemble de routeurs vers les headers
   concrets.
3. Clarifier la position architecturale de `tm_libc` et son rapport au HAL et à `sysCall`.
4. Séparer les contrats portables de la configuration kernel générée.
5. Valider la topologie avec une deuxième cible avant toute multiplication des sous-dossiers.

L'arborescence actuelle doit donc être conservée comme squelette. Les prochains efforts devraient
porter sur la conformité des dépendances à ce squelette, plutôt que sur un renommage ou un déplacement
général des fichiers.
