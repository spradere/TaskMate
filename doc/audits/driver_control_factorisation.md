# Audit de factorisation de `hal_*GetStatus()` et `hal_*Control()`

## Objet et périmètre

Cet audit évalue la factorisation des six pilotes enregistrés sur la cible actuelle
`avr8 / atmega2560 / arduinoMega` : I2C, USART, timer d'ordonnancement, timer STC, LCD
AMC2004 et RTC ZS042. Il porte sur les fonctions privées `hal_*GetStatus()` et les points
d'entrée publics `hal_*Control()` ; il ne propose pas de modifier les fonctions métier des
pilotes.

Les chiffres ci-dessous sont des estimations statiques pour AVR8 avec les options actuelles
(`-Os`, `-fshort-enums`, sections séparées et LTO). Ils donnent des ordres de grandeur, pas une
mesure du binaire : la chaîne AVR n'est pas disponible dans l'environnement de l'audit. Toute
implémentation devra donc être comparée au binaire de référence avec `avr-size`, `avr-nm` et
`avr-objdump`.

## Constat

Les six fonctions `Control()` ont le même protocole :

1. trois commandes de cycle de vie spécifiques (`INIT`, `START`, `STOP`) ;
2. sept commandes génériques qui lisent ou modifient le niveau d'exécution, les bits d'état,
   l'état calculé et la dernière erreur ;
3. les mêmes contrôles de pointeur, de niveau et d'indice de bit ;
4. une erreur `ERR_HAL_DRIVER_INVALID_CONTROL` pour toute autre commande.

Cela représente six copies d'un bloc d'environ 55 lignes, dont environ 45 lignes sont purement
génériques. Les six `GetStatus()` partagent aussi la même machine à états locale : priorité à
`DEAD`, puis `ERROR`, cohérence de `INIT`/`START`, puis conversion en état public.

La similitude n'est toutefois pas totale :

- LCD et RTC vérifient l'état du pilote I2C **après** `DEAD` et `ERROR`, mais **avant** les bits
  `INIT` et `START` ;
- chaque pilote conserve son état et sa dernière erreur dans des objets privés ;
- la dernière erreur USART est `volatile` car elle est aussi écrite depuis l'ISR RX ;
- les opérations `INIT`, `START` et `STOP` restent spécifiques au matériel et peuvent toucher des
  registres volatils ou activer des interruptions.

La priorité des tests fait partie du comportement observable : déplacer la vérification de la
dépendance I2C avant `DEAD`, par exemple, peut remplacer une erreur fatale locale par une erreur de
dépendance. Une factorisation ne doit donc pas réduire la machine à états à une simple table sans
préserver cet ordre.

## Options étudiées

### 1. Macro ou génération de source

Un gabarit de préprocesseur, un fichier inclus paramétré ou une génération à la compilation peut
produire les corps actuels en injectant les noms de l'état, de l'erreur et des trois fonctions de
cycle de vie.

| Effet | Estimation |
|---|---:|
| Flash | 0 octet (à quelques octets d'optimisation LTO près) |
| RAM statique | 0 octet |
| Pile | 0 octet |
| Temps CPU | 0 cycle |
| Source manuscrite | environ 250 à 300 lignes supprimées |

Cette option garantit le comportement binaire le plus proche, mais déplace la complexité vers le
préprocesseur ou le générateur. Elle rend les diagnostics et le débogage moins directs. Étendre
autoCode uniquement pour ces six pilotes serait disproportionné ; un générateur ne devient
intéressant que si le nombre de pilotes augmente nettement ou si leurs déclarations deviennent déjà
des données de configuration.

### 2. Fonctions communes à l'exécution, sans descripteur persistant

Une unité HAL commune peut fournir :

- un évaluateur de la machine à états locale prenant les adresses de l'octet d'état et de la dernière
  erreur ;
- un traitement des sept commandes génériques ;
- un petit wrapper par pilote qui traite `INIT`, `START` et `STOP`, puis appelle le traitement commun.

LCD et RTC conserveraient un wrapper d'état pour insérer la vérification I2C au point exact de la
séquence. Les pointeurs peuvent être passés à chaque appel afin de ne créer aucune table en RAM.

| Effet total pour six pilotes | Estimation |
|---|---:|
| Flash | gain net de 350 à 650 octets |
| RAM statique | 0 octet |
| Pile maximale | +2 à +8 octets selon l'allocation des registres/LTO |
| `GETSTATUS` simple | +8 à +25 cycles |
| commande générique | +12 à +35 cycles |
| commande de cycle de vie | +8 à +20 cycles |

Les bornes incluent un appel/retour supplémentaire, le chargement des adresses et, suivant les
décisions de LTO, d'éventuels spills sur la pile. Elles excluent le temps propre aux périphériques ;
pour LCD/RTC, l'interrogation I2C domine largement ce surcoût. Le gain flash est plausible parce que
le gros `switch` n'existe plus qu'une fois, mais LTO peut ré-inliner le helper et annuler une partie
du gain. Le helper devra donc être mesuré avec et sans attribut empêchant l'inlining, sans imposer cet
attribut avant d'avoir observé le désassemblage.

### 3. Descripteur générique avec callbacks

Un descripteur par pilote pourrait contenir les adresses de l'état, de la dernière erreur et des
callbacks `init/start/stop`, éventuellement un callback de dépendance. Cette solution réduit encore
les wrappers et facilite l'ajout de pilotes.

Sur ATmega2560, six descripteurs nécessiteraient environ 60 à 84 octets selon le nombre et la taille
effective des pointeurs. En C AVR, un objet `const` ordinaire n'est pas une garantie de zéro RAM : il
peut être copié de la flash vers `.data`. Le placer explicitement en mémoire programme supprime ce
coût RAM, mais impose des lectures `pgm_read_*`, complique la portabilité et ajoute des cycles.

| Effet total pour six pilotes | Estimation |
|---|---:|
| Flash, descripteurs inclus | gain net de 400 à 750 octets |
| RAM statique, `const` ordinaire | coût de 60 à 84 octets |
| RAM statique, stockage programme explicite | 0 octet |
| Pile maximale | +4 à +10 octets |
| Commande | +20 à +55 cycles, hors opération matérielle |

Le faible gain flash supplémentaire ne justifie pas 60 à 84 octets de RAM sur une cible de 8 Kio,
ni une nouvelle abstraction `PROGMEM` transversale. Les appels indirects rendent aussi l'analyse du
graphe d'appel et du temps maximal plus difficile. Cette option est donc rejetée pour la cible
actuelle.

### 4. `static inline` commun

Un helper `static inline` dans un header améliore la maintenance du texte mais autorise une copie
spécialisée dans chaque unité de traduction. Avec `-Os` et LTO, le résultat peut aller d'une bonne
mutualisation à une duplication complète ; ni le gain flash ni le temps CPU ne sont prévisibles.
Cette option n'est pas retenue comme stratégie d'optimisation mesurable.

## Contraintes fonctionnelles et de concurrence

- L'octet d'état est atomique sur AVR8, mais une factorisation doit conserver les accès aux registres
  et aux états dans leur ordre actuel. Elle ne doit pas introduire de section critique générique.
- La dernière erreur USART doit rester `volatile`. Une API commune ne doit pas faire disparaître ce
  qualificateur par conversion de pointeur ; il faut soit un chemin dédié, soit un contrat acceptant
  explicitement un pointeur volatile.
- `GETLASTERROR` n'est pas une lecture sans effet : l'appel final à `GetStatus()` peut remplacer
  l'erreur retournée dans l'état interne si les bits décrivent un état invalide. Le comportement doit
  être couvert par des tests avant refactoring.
- `SETBIT` et `CLEARBIT` exposent `INIT`, `START`, `ERROR` et `DEAD`. Toutes les combinaisons, y compris
  `START=1` avec `INIT=0`, doivent garder le même résultat et la même dernière erreur.
- Le code commun appartient à une unité HAL neutre, consommable par `mcu/` et `drivers/`. Il ne doit
  pas être placé dans `interfaces/`, qui ne contient que le contrat portable et aucune logique
  d'exécution.

## Verdict et recommandation

La factorisation est **techniquement faisable**. Pour six pilotes, la meilleure balance est l'option
2 : un helper HAL compilé une seule fois, aucun descripteur persistant, et des wrappers explicites
pour le cycle de vie et les dépendances. Le gain attendu est de **350 à 650 octets de flash**, sans
RAM statique supplémentaire, au prix d'environ **8 à 35 cycles** et de quelques octets de pile par
commande. Ce coût est acceptable pour les commandes de gestion, qui ne sont pas sur le chemin de
l'ISR d'ordonnancement ; il faut néanmoins éviter d'utiliser le helper générique depuis une ISR.

Si l'objectif prioritaire est uniquement de supprimer la répétition sans aucune variation du
binaire ou du WCET, l'option 1 est préférable, mais un petit gabarit local est recommandé plutôt
qu'une extension d'autoCode.

La mise en œuvre devrait être séparée en deux étapes mesurables :

1. mutualiser et tester seulement la machine `GetStatus()` ;
2. mutualiser ensuite les sept commandes génériques de `Control()`.

Un seuil d'acceptation raisonnable pour conserver le refactoring est un gain mesuré d'au moins
**256 octets de flash**, **0 octet de RAM statique**, et un surcoût maximal de **40 cycles** pour une
commande générique. En dessous de ce gain, la version générée à coût d'exécution nul est plus adaptée.

## Plan de validation obligatoire

1. Construire la référence propre avec `bmake clean && bmake` et archiver ELF, map et sortie
   `avr-size`.
2. Ajouter des tests de table pour les 16 combinaisons des quatre bits d'état, avec et sans dépendance
   I2C disponible, et vérifier état retourné **et** dernière erreur.
3. Construire chaque étape avec exactement les mêmes options et comparer `.text`, `.data`, `.bss`,
   les symboles et le désassemblage.
4. Compter les cycles sur les chemins `GETSTATUS`, `RLGET`, `SETBIT`, commande invalide et
   `INIT`, en incluant prologue, épilogue et appels indirects éventuels.
5. Rechercher les appels depuis ISR et contrôler la profondeur de pile sur le pire chemin.
6. Exécuter `bmake cppcheck` et la vérification des frontières incluse dans `bmake`.
7. Valider enfin sur Arduino Mega les transitions de niveau, l'USART sous interruption, les timers,
   puis la perte de dépendance I2C vue par LCD et RTC.
