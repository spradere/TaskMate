# Audit de finalisation du refactor `tm_string_t`

Date : 14 septembre 2026

Révision de base : `9c64446`

Branche auditée : `codex/tm-string`

État audité : révision intégrant le déplacement vers `sysCall/sc_string.*`

## Objet et règles cibles

Cet audit vérifie le chemin complet des chaînes, depuis leur définition jusqu'aux sorties LCD et
USART. Il couvre `interfaces`, `sysCore`, `sysCall`, `tmLibc`, les services, autoCode et le HAL. Il
ne modifie pas le firmware ni les régions générées.

Les règles retenues sont :

1. `interfaces` contient seulement les types et constantes communs, sans choix de stockage ;
2. `sysCall` est l'unique couche qui interprète l'emplacement RAM, ROM ou futur ;
3. le HAL reçoit et émet uniquement des octets `uint8_t` ;
4. les couches au-dessus de `sysCall` manipulent une chaîne sans tester ni nommer son stockage.

## Verdict

Le refactor est **engagé mais pas finalisé**. La frontière d'exécution du HAL est maintenant
orientée octets : aucun fichier HAL ne consomme `tm_string_t`, `TM_MEM_*`, `PROGMEM`, `PSTR` ou une
API `*WriteString`. En revanche, trois défauts structurants restent ouverts :

- `sysCall` dépend encore de `tmLibc`, alors que `tmLibc` dépend désormais de `sysCall` ;
- l'en-tête public `sc_string.h` expose directement RAM, ROM et les primitives AVR ;
- les noms générés des threads et pilotes sont encore construits dans `sysCore` avec une macro ROM.

La cible ne sera cohérente qu'après suppression du cycle `sysCall <-> tmLibc` et déplacement de
toute décision de stockage hors de `sysCore`, des services et de `interfaces/tm_macros.h`.

## Chemins actuels

Le chemin de formatage normal est désormais :

```text
service -> tmLibc -> sc_stringGetChar()/sc_stdioPutChar()
                    -> HAL USART uint8_t -> matériel
```

Le chemin LCD est correctement remonté au-dessus du HAL :

```text
service -> sc_lcdWriteString(tm_string_t)
        -> sc_stringGetChar()
        -> hal_lcdWriteStart/Char/End(uint8_t)
        -> I2C
```

Mais les recherches de modules suivent encore le chemin inverse :

```text
sysCall -> tm_strncmp()/TM_STR_RAM -> sc_stringGetChar() -> sysCall
```

`sc_i2cScan()` ajoute un second retour vers le haut par `tm_syslog()`. Ce cycle est compilable en C,
mais contredit la matrice d'architecture et empêche `sysCall` d'être la couche propriétaire.

## Conformité par couche

| Couche | État | Constat |
| --- | --- | --- |
| `interfaces` | Partiel | `tm_string.h` est neutre ; une autre macro nomme encore la ROM. |
| `sysCall` | Partiel | Lit RAM/ROM et pilote les sorties, mais appelle encore `tmLibc`. |
| `HAL` | Presque conforme | Contrats en `uint8_t`, aucun descripteur de chaîne restant. |
| `sysCore` | Non conforme | Les records possèdent encore des noms créés avec une macro ROM. |
| `tmLibc` | Partiel | Délègue la lecture à `sysCall`, mais réexporte les macros de stockage. |
| Services/tâches | Non conforme | Seize appels actifs utilisent explicitement `TM_STR_RAM`. |

## Écarts à corriger

### P0 — Supprimer le cycle `sysCall <-> tmLibc`

`sc_driver.c` inclut `tm_string.h` et `tm_syslog.h`, `sc_threads.c` inclut `tm_string.h`, et `sc_errors.c`
inclut aussi ce dernier uniquement pour créer ses textes. Dans l'autre sens, `tm_string.c` et
`tm_snprintf.c` appellent `sc_stringGetChar()` ou `sc_stdioPutChar()`.

La correction recommandée est de fournir dans `sc_string.*` les primitives dépendantes du stockage :

```c
uint8_t sc_stringGetByte(const tm_string_t *string, uint8_t index);
int sc_stringCompare(tm_string_t left, tm_string_t right, uint8_t size);
void sc_stringCopy(char *dest, tm_string_t src, uint8_t size);
```

`tm_strncmp()` et `tm_strncpy()` peuvent rester comme façades `tmLibc` et déléguer à ces primitives.
Les recherches internes de pilotes et threads appellent directement `sc_stringCompare()`. La trace
de découverte I2C doit être remontée au service, exposée comme résultat borné, ou supprimée ; un
syscall ne doit pas appeler `tm_syslog()`.

### P0 — Masquer le stockage aux couches hautes

`sc_string.h` inclut actuellement `<avr/pgmspace.h>` et publie `TM_STR_ROM`, `TM_STR_RAM` et
`TM_STR_ROM_NEW`. Les services contiennent seize utilisations actives de `TM_STR_RAM`. Ils savent
donc encore quel stockage décrire, même s'ils ne lisent jamais directement le champ `storage`.

L'API haute doit exprimer l'origine logique, pas l'espace d'adressage :

- conserver `TM_STR("literal")` pour un littéral dont le placement est choisi par la cible ;
- remplacer `TM_STR_RAM(buffer)` par un constructeur tel que `sc_stringFromBuffer(buffer)` ;
- réserver une macro statique sans suffixe de stockage aux catalogues possédés par `sysCall` ;
- rendre privées les macros contenant `ROM`, `RAM`, `PSTR`, `PROGMEM` ou `TM_MEM_*`.

Le descripteur reste défini dans `interfaces/tm_string.h`, mais une règle lexicale doit interdire
aux couches hautes l'accès à `.storage` et aux constantes `TM_MEM_*`. Rendre le type opaque n'est
pas nécessaire à ce stade et augmenterait fortement la propagation en C embarqué.

### P1 — Sortir les noms de modules de `sysCore`

autoCode émet dix `TM_STR_ROM_NEW` dans `sysCore/sys_modules.c`. Les champs `name` des records thread et
driver ne sont lus que par `sysCall`. Cette propriété permet une séparation nette :

- `sysCore` conserve les états, piles, callbacks, adresses et identifiants numériques ;
- `sysCall` possède deux catalogues générés de noms, indexés par identifiant de thread ou pilote ;
- `sc_threadGetInfo()`, `sc_driverGetInfo()` et les recherches par nom consultent ces catalogues ;
- autoCode génère les textes dans un fichier source `sysCall`, jamais dans `sysCore`.

Les régions existantes étant générées, la migration doit modifier `parseTag.c` et ses sources de
vérité, puis exécuter autoCode deux fois. Il ne faut pas éditer `modules.c` ou `error.c` à la main.

### P1 — Nettoyer la frontière HAL octet

Les types sont corrects, mais les symboles `hal_usartWriteChar()` et `hal_lcdWriteChar()` parlent
encore de caractères. Les renommer `hal_usartWriteByte()` et `hal_lcdWriteByte()` rendrait le
contrat explicite et facilement contrôlable.

Le protocole LCD `WriteStart/WriteChar/WriteEnd` préserve une seule transaction I2C, mais expose un
état séquencé. Toute erreur entre start et end quitte actuellement `sc_lcdWriteString()` sans appel
de fin. La finalisation doit garantir l'arrêt I2C sur chaque sortie d'erreur et documenter que cette
séquence n'est ni réentrante ni utilisable depuis une ISR.

### P1 — Clarifier la sortie console

`sc_stdioPutChar(char)` combine une notion de caractère, la politique de flush à la fin de ligne et
le transport USART. Le HAL reste byte-only, mais le nom et le contrat cachent mal la frontière.

Une API `sc_consoleWriteByte(uint8_t)` et `sc_consoleFlush()` sépare mieux :

- `tmLibc` décide quels octets produit le formatage ;
- `sysCall` décide du périphérique et traduit ses erreurs ;
- USART met en file et émet des octets, sans connaître NUL ni fin de ligne.

Le boot utilise cette sortie avant l'ordonnanceur. L'implémentation ne doit donc pas attendre un
yield, allouer dynamiquement ou supposer qu'un thread courant existe.

### P2 — Durcir les primitives de chaîne

`sc_stringGetChar()` ne valide pas le pointeur de descripteur avant de lire `str->storage`. Certains
appelants vérifient `text`, mais le formateur `%s` peut encore recevoir un pointeur nul. La primitive
centrale doit définir les cas `string == NULL`, `text == NULL`, stockage inconnu et index maximal.

Le type de retour recommandé à la frontière basse est `uint8_t`. Une conversion en `char` reste
possible dans `tmLibc` lorsqu'une sémantique textuelle l'exige. Cela évite les ambiguïtés de signe
sur AVR et aligne la fonction sur la règle HAL orientée octets.

La branche `TM_LIBC_CSTD` doit aussi être compilée séparément : ses macros retournent des pointeurs
C alors que plusieurs APIs système attendent encore `tm_string_t`. Elle ne doit pas être considérée
valide sur la seule base du build TaskMate courant.

## Architecture cible

```text
interfaces/tm_string.h
    descripteur et limites neutres uniquement
             |
             v
sysCall/sc_string
    création opaque, lecture, comparaison, copie RAM/ROM
    catalogues générés de noms et erreurs
             |
             +---------------------> HAL LCD/USART : uint8_t seulement
             ^
             |
tmLibc : formatage et façades C bornées
             ^
             |
services et tâches : TM_STR(littéral) ou constructeur de buffer
```

Aucune flèche ne doit remonter de `sysCall` vers `tmLibc`, et aucun fichier HAL ne doit inclure
`sc_string.h` ou `tm_string.h`.

## Ordre de mise en œuvre recommandé

1. Ajouter les primitives `sc_stringCompare/Copy/GetByte` et convertir les appels internes de
   `sysCall` ; déplacer ou retirer la trace de `sc_i2cScan()`.
2. Faire de `tm_strncmp/tm_strncpy` des façades descendantes, puis vérifier l'absence de tout include
   `tmLibc/` sous `sysCall`.
3. Remplacer les macros publiques orientées stockage par `TM_STR` et un constructeur de buffer sans
   notion RAM ; adapter services et formateur.
4. Déplacer les catalogues générés de noms de `sysCore` vers `sysCall` en modifiant autoCode.
5. Retirer `TM_STORE_FILE_NAME` de `interfaces/tm_macros.h` ou le remplacer par une construction
   publique sans mention de ROM.
6. Renommer les opérations HAL `*Char` en `*Byte` et fiabiliser la fermeture de transaction LCD.
7. Ajouter les contrôles de dépendances et les tests de stockage avant toute autre abstraction.

Cet ordre ferme d'abord le cycle interdit, puis masque le stockage, sans imposer simultanément une
refonte du formateur ou de tous les appels texte.

## Contrôles à ajouter

Les recherches négatives suivantes doivent devenir des règles de build :

```text
HAL       : tm_string_t, TM_STR*, TM_MEM_*, PROGMEM, PSTR, pgm_read, *String, *Char
sysCall   : #include "tmLibc/
sysCore   : TM_STR_RAM, TM_STR_ROM, TM_MEM_*, PROGMEM, PSTR
couches hautes : TM_STR_RAM, TM_STR_ROM, TM_MEM_*, .storage, PROGMEM, PSTR, pgm_read
interfaces: include vers sysCall/tmLibc/HAL et macro dépendant d'un stockage concret
```

Les tests fonctionnels minimaux doivent couvrir : chaîne vide, pointeur nul, taille zéro, limite
`TM_STRING_SIZE_MAX`, octets supérieurs à `0x7f`, comparaison RAM/RAM, RAM/ROM et ROM/ROM, copie avec
terminaison NUL, format `%s`, saturation USART et erreur I2C pendant une écriture LCD.

Après chaque modification autoCode :

1. exécuter `bmake autoCode_alone` deux fois et obtenir `0 updated` au second passage ;
2. exécuter les tests autoCode et les tests sémantiques de chaînes ;
3. construire proprement `test1` et `VAL_TARGET=test_noscli` ;
4. lancer `bmake cppcheck` et `git diff --check` ;
5. comparer flash, SRAM et pile avec l'ELF de référence ;
6. valider sur Arduino Mega le boot précoce, USART, LCD et les commandes SCLI.

## Critères de clôture

Le refactor pourra être déclaré terminé lorsque :

- `interfaces/tm_string.h` reste neutre et aucune autre interface ne choisit RAM ou ROM ;
- seul `sysCall` contient les décisions `TM_MEM_*`, `PROGMEM`, `PSTR` et `pgm_read_*` ;
- aucune source HAL ne contient de type, fonction ou nom lié aux chaînes ou caractères ;
- aucune source `sysCall` n'inclut ni n'appelle `tmLibc` ;
- les couches hautes n'utilisent que `TM_STR` et un constructeur sans stockage explicite ;
- les noms de modules et erreurs sont générés dans une unité appartenant à `sysCall` ;
- les deux builds AVR, les tests, l'analyse statique et la validation matérielle passent.
