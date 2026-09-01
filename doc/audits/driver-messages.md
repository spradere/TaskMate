# Audit — Messages de diagnostic des drivers

Date : 2026-09-01
Révision auditée : `4ea6a53`

## Objet

Cet audit étudie des solutions simples permettant aux drivers TaskMate de produire des messages de
diagnostic sans violer les frontières entre HAL, `sysCall`, services et tâches. Il tient compte des
contraintes de l'ATmega2560 : RAM limitée, temps d'exécution borné, contexte d'interruption,
atomicité et dépendances de démarrage.

L'étude est en lecture seule. Les solutions décrites ci-dessous sont des propositions et ne sont pas
implémentées par ce rapport.

## Constat actuel

Le driver I2C inclut `tm_libc/tm_syslog.h` et appelle `tm_syslog()` pendant son démarrage pour
annoncer le scan puis chaque adresse détectée :

- `srcs/hal/mcu/atmega2560/i2c.c:24` ;
- `srcs/hal/mcu/atmega2560/i2c.c:99` ;
- `srcs/hal/mcu/atmega2560/i2c.c:108`.

Ce chemin paraît court dans le driver, mais son exécution traverse plusieurs couches :

```text
I2C -> tm_syslog -> tm_vprintf -> hal_stdio_putChar -> USART
                         |
                         +-> sc_coopYield en cas de contention
```

Les éléments qui construisent ce chemin sont les suivants :

- `tm_syslog()` transmet les arguments à `tm_vprintf()` dans
  `srcs/tm_libc/tm_syslog.c:19-27` ;
- le formateur inclut `system/sysCall/sysCall.h` et appelle `sc_coopYield()` lorsque son verrou global
  est déjà pris dans `srcs/tm_libc/tm_snprintf.c:17-20` et `:104-107` ;
- chaque caractère sans tampon de destination est envoyé à `hal_stdio_putChar()` dans
  `srcs/tm_libc/tm_snprintf.c:90-102` ;
- le backend ATmega2560 écrit dans le driver USART et force un envoi sur fin de ligne dans
  `srcs/hal/mcu/atmega2560/tmlibc.c:19-32` ;
- `hal_usartSendTXBuffer()` attend activement que le registre matériel accepte chaque octet dans
  `srcs/hal/mcu/atmega2560/usart.c:173-185`.

Le driver I2C dépend donc implicitement de l'USART, du formateur global et, en cas de contention, de
l'ordonnanceur. Cette dépendance est particulièrement fragile pendant le boot, avant le démarrage de
l'ordonnanceur, et dans une interruption.

Le formateur n'est pas réentrant : il utilise un buffer et un verrou globaux non atomiques. Un
diagnostic émis par le driver USART lui-même pourrait également revenir vers ce même driver et créer
une récursion ou un blocage.

## Mécanismes déjà disponibles

TaskMate possède déjà l'essentiel d'un contrat de diagnostic par erreur :

- chaque driver retourne un `hal_driver_state_t` ;
- chaque driver conserve un `err_codes_t` privé ;
- `DRV_CTRL_GETLASTERROR` permet de récupérer la dernière erreur ;
- les fichiers `*.err` sont agrégés par autoCode ;
- `err_getMessage()` retrouve le texte ROM associé à un code.

En revanche, `boot()` ignore actuellement les résultats de `DRV_CTRL_INIT` et `DRV_CTRL_START`, puis
affiche systématiquement `start ... ok` dans `srcs/system/sysCore/boot.c:56-63`.

## Proposition 1 — Le driver signale, l'appelant affiche

Cette proposition réutilise le contrat actuel. Un driver ne produit aucune sortie texte : il conserve
son erreur et retourne son état. L'appelant situé dans `sysCore`, `sysCall` ou un service récupère
ensuite `DRV_CTRL_GETLASTERROR`, résout le texte avec `err_getMessage()` et décide de l'affichage.

Le chemin devient :

```text
driver -> état + code d'erreur -> sysCore/sysCall/service -> tm_syslog -> USART
```

Les changements attendus seraient :

1. supprimer les appels à `tm_syslog()` des drivers ;
2. vérifier les résultats des contrôles de cycle de vie dans `boot()` ;
3. afficher le nom du driver et le message du catalogue uniquement dans la couche appelante ;
4. déplacer le scan I2C hors de `hal_i2cStart()` ;
5. exposer ce scan comme une opération diagnostique explicite appelée par SCLI à travers `sysCall`.

Avantages :

- presque aucune consommation RAM supplémentaire ;
- aucune dépendance driver vers `sysCall`, service ou tâche ;
- aucune récursion avec l'USART ;
- comportement utilisable pendant le boot ;
- réutilisation du catalogue d'erreurs et du contrat homogène des drivers.

Limite : seule la dernière erreur de chaque driver est conservée. Cette solution ne fournit pas
d'historique pour les événements asynchrones.

## Proposition 2 — File fixe de diagnostics structurés

Cette solution répond au besoin d'événements asynchrones ou produits depuis une ISR. Le driver ne
formate toujours aucun texte : il insère un petit enregistrement dans une file statique.

```text
driver -> tm_diagReport(record)
                       |
                  file fixe
                       |
service diagnostic -> err_getMessage -> tm_syslog -> USART
```

Un enregistrement minimal pourrait prendre la forme suivante :

```c
typedef struct
{
	uint8_t source;
	uint8_t event;
	uint16_t detail;
} tm_diag_record_t;
```

Contraintes recommandées :

- file de 8 ou 16 entrées allouée statiquement ;
- insertion en temps borné, sans attente et sans formatage ;
- section atomique courte couvrant la copie et la mise à jour des index ;
- politique de débordement explicite, par exemple abandon du nouvel événement ;
- compteur saturant des événements perdus ;
- aucun appel à `sc_*`, au formateur ou à l'USART dans le chemin producteur ;
- vidage et affichage uniquement par un service système.

Un composant `tm_diag` isolé peut appartenir à la couche transversale `tm_libc`, à condition de rester
indépendant du formateur actuel. Les types portables peuvent être placés dans `interfaces/`, mais
aucun état ni traitement d'exécution ne doit y être ajouté.

Sur l'AVR actuel, huit enregistrements représenteraient environ 32 à 40 octets, auxquels s'ajoutent
les index et le compteur de pertes. La taille et le coût flash doivent être vérifiés avec `avr-nm` et
`avr-size`, sans imposer de structure compactée non portable.

Cette solution nécessite également une source stable pour les identifiants de driver et d'événement.
Une génération depuis les fichiers de configuration éviterait une table manuelle divergente, mais
transformerait le changement en évolution autoCode, donc en modification système critique.

## Proposition 3 — Console d'urgence inspirée de Xinu

Xinu distingue les sorties ordinaires de fonctions noyau telles que `kprintf()` et `kputc()`. Ces
fonctions sont destinées au diagnostic lorsque le sous-système normal d'E/S ou les interruptions ne
sont pas disponibles.

Sources :

- [API noyau Embedded Xinu](https://github.com/xinu-os/xinu/blob/master/include/kernel.h) ;
- [supports de cours Xinu de Purdue](https://www.cs.purdue.edu/homes/comer/downloads/private/Xinu_Book_Slides/Xinu_Instructor_Slides.pdf).

TaskMate pourrait reprendre cette séparation avec une console d'urgence très limitée :

- écriture USART directe par polling dans `hal/mcu/<mcu>/` ;
- sélection de la capacité par `hal/public/` ;
- petit formateur réentrant à état local, ou seulement une écriture de chaînes ROM ;
- activation par option de compilation ;
- utilisation limitée au boot précoce et à `panic()`.

Cette console ne doit pas devenir le mécanisme normal des drivers. Une sortie par polling est
bloquante et allonge fortement la latence des interruptions. Elle doit aussi posséder explicitement
l'USART pendant son utilisation afin de ne pas concurrencer le buffer du driver normal.

## Solution déconseillée — Autoriser directement `tm_syslog()` dans les drivers

Cette approche est courte à écrire mais conserve tous les défauts constatés :

- dépendance HAL vers `sysCall` à travers le formateur ;
- dépendance implicite de tous les drivers vers l'USART ;
- formateur global non réentrant ;
- blocage possible lors du vidage USART ;
- utilisation invalide ou indéfinie avant l'ordonnanceur et dans une ISR ;
- risque de récursion pour les diagnostics de l'USART.

Elle ne respecte donc ni les frontières TaskMate ni les contraintes de déterminisme attendues d'un
RTOS embarqué.

## Recommandation

L'implémentation la plus simple et la plus cohérente consiste à commencer par la proposition 1 :

1. les drivers signalent exclusivement des états et des codes ;
2. `boot`, `sysCall` ou les services effectuent l'affichage ;
3. le scan I2C devient une commande diagnostique explicite et non une action cachée de `START`.

La file structurée de la proposition 2 doit être ajoutée seulement si des événements asynchrones ou
ISR doivent réellement être conservés. La console inspirée de Xinu doit rester indépendante et
réservée aux pannes fatales ou au démarrage très précoce.

## Validation attendue lors de l'implémentation

- recherche négative de `tm_syslog`, `tm_printf` et `tm_vprintf` dans `srcs/hal/` ;
- tests du boot avec succès et échec de chaque cycle de vie driver ;
- test USART arrêté, buffer plein et erreur du driver USART lui-même ;
- si une file est ajoutée : tests de débordement, ordre, compteur de pertes et émission ISR ;
- vérification des allowlists de headers et des gardes de compilation ;
- régénération autoCode si ses sources sont modifiées, puis contrôle du log et du diff généré ;
- `bmake clean` suivi de `bmake` ;
- comparaison RAM/flash avec `avr-size` et contrôle des symboles avec `avr-nm` ;
- essai sur Arduino Mega pour vérifier la latence série et l'absence de perturbation des timers.
