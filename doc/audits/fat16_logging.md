# Audit de faisabilité — journalisation FAT16 sur carte SD

## Résumé exécutif

L'ajout est **faisable sur l'ATmega2560**, mais il ne s'agit pas seulement d'ajouter un parseur
FAT16 : TaskMate ne possède actuellement ni pilote SPI, ni abstraction de périphérique bloc, ni
service de stockage. Le chemin recommandé est une carte SD en mode SPI, un pilote bloc dans le HAL,
puis un service de journalisation possédant un sous-ensemble FAT16 volontairement réduit.

Pour le besoin « produire sur la carte un fichier directement lisible par un PC », le meilleur
compromis n'est pas un FAT16 généraliste. Une première version doit accepter un volume FAT16 créé
sur PC, un seul fichier 8.3 préalloué et contigu, et uniquement l'ajout séquentiel. Elle met à jour
la taille du fichier lors d'un `sync`, sans allocation dynamique, répertoire, suppression, renommage
ni accès concurrents. Ce périmètre est de **complexité moyenne à élevée**, estimé à **15 à 25 jours
ingénieur**, plus **3 à 5 jours** de validation sur carte et de tests de coupure d'alimentation.

Estimation rapide pour cette version, à confirmer par compilation et mesures :

| Ressource | Coût incrémental réaliste | Hypothèse |
|---|---:|---|
| SRAM statique et pile | **0,9 à 1,5 Kio** | cache secteur unique de 512 octets, file de logs bornée et pile du service |
| Flash | **6 à 12 Kio** | SPI + protocole SD + FAT16 réduit + API/service et erreurs |
| CPU pendant un transfert | **proche de 100 %** | SPI interrogé, aucune interruption/DMA sur AVR8 |
| CPU moyen à 1 Kio/s de logs | **environ 1 à 5 %** | dépend surtout des latences internes de la carte et de la politique de `sync` |
| Blocage par secteur | **environ 1 ms au fil à 4 MHz**, souvent plusieurs ms en écriture | pointes carte possibles de dizaines à centaines de ms |

La SRAM est la contrainte principale : la référence déjà auditée consomme 1 957 octets sur 8 192,
piles existantes comprises. Le budget proposé peut tenir, mais il augmente sensiblement la pression
sur les piles et interdit les buffers dupliqués. La limite logicielle actuelle de 64 Kio de flash
laisse plus de marge, bien qu'elle n'exploite pas toute la flash physique de l'ATmega2560.

**Décision recommandée : GO conditionnel**, précédé d'un prototype SPI/SD bloc mesurant les latences
réelles avec la carte choisie. Une exigence de garantie temps réel stricte, d'écriture dans plusieurs
fichiers, ou de résistance complète à toute coupure ferait passer la recommandation à un système plus
large et plus coûteux.

## Périmètre et constats dans TaskMate

L'audit porte sur la cible par défaut `test1 / arduinoMega / atmega2560 / avr8` et la révision
`c7a7b6f`. Il s'appuie sur la structure et les sources du dépôt, ainsi que sur l'audit mémoire
existant. La construction n'a pas pu être rejouée dans l'environnement d'audit car `bmake` n'y est
pas installé ; les chiffres ajoutés sont donc des fourchettes d'architecture, pas une mesure du
binaire final.

Constats déterminants :

- le MCU est configuré avec 8 Kio de SRAM et une limite de build de 64 Kio de flash, due au contexte
  AVR qui ne sauvegarde pas `PCHH` ;
- le HAL public sélectionne actuellement GPIO, I2C, USART, timers, RTC et LCD, mais aucun SPI ni
  périphérique bloc ;
- les drivers sont enregistrés au démarrage via autoCode et les fichiers `*.rc` ;
- les services ne doivent pas inclure le HAL. Ils utilisent les syscalls, comme SCLI utilise
  `sc_usartRead()` ;
- `tm_syslog()` formate aujourd'hui directement vers `tm_vprintf()` et le backend USART. Il ne
  possède ni notion de destination, ni file persistante, ni résultat de livraison ;
- `sc_coopYield()` existe, mais les transactions I2C/USART observées sont largement interrogées ou
  bloquantes. Il n'existe ni mutex générique, ni attente d'événement de driver ;
- l'audit mémoire existant mesure 1 957 octets de SRAM, dont 1 024 octets de piles de quatre threads,
  et 8 724 octets de flash à une révision antérieure. Ces valeurs donnent l'ordre de grandeur, pas
  un nouveau point de référence.

La carte Arduino Mega expose le SPI matériel sur les broches 50 à 53. La configuration cible devra
réserver MISO, MOSI, SCK et un chip-select SD et vérifier les conflits électriques avec les signaux
existants et les autres périphériques du shield. Ce choix relève de la cible/carte, pas du code FAT.

## Architecture proposée

### 1. HAL MCU : transport SPI

Ajouter `hal/public/spi.h` et l'implémentation ATmega2560 dans le répertoire MCU. L'API doit rester
orientée transfert, bornée et synchrone dans un premier temps : initialisation lente pour la carte,
changement vers l'horloge normale, échange d'un octet ou d'un bloc, et délais d'expiration exprimés
en unités explicites.

Le pilote doit gérer les contraintes AVR suivantes : mode maître conservé par la broche SS, registres
`volatile`, ordre SPDR/SPIF, fréquences issues de `F_CPU`, et aucune attente infinie. Le chip-select
physique et une éventuelle détection de carte appartiennent à la configuration board/target.

### 2. HAL driver : périphérique bloc SD

Ajouter un driver réutilisable `hal/drivers/sd_spi/` qui traduit des numéros de secteurs
`uint32_t` en commandes SD. Il expose au minimum :

- initialisation SD v1/v2 et SDHC/SDSC ;
- lecture d'un secteur de 512 octets ;
- écriture d'un secteur de 512 octets ;
- attente « ready » avec timeout ;
- synchronisation/état, retrait et erreurs explicites.

Le driver ne connaît ni FAT ni noms de fichiers. Pour SDSC, la conversion secteur/adresse doit être
faite en 32 bits et contrôlée. Les CRC peuvent être exigés seulement pendant l'initialisation en V1,
mais activer/vérifier le CRC en exploitation augmente la robustesse au prix de flash et CPU.

### 3. Contrat bloc portable

Définir dans `interfaces/` seulement les types neutres nécessaires : numéro de LBA, taille logique de
secteur, résultats et capacités. L'implémentation et les détails SD restent dans le HAL. Une API bloc
publique côté HAL permet ensuite à `sysCall` de déléguer sans rendre FAT dépendant de l'ATmega2560.

Il faut étendre les allowlists de headers au lieu de contourner les frontières. SPI et SD doivent être
sélectionnés par l'arbre et les fichiers Make de la cible ; aucun `#if MCU_atmega2560` ne doit entrer
dans le code FAT portable.

### 4. Service stockage/journal

Placer la politique FAT16 et la propriété du volume dans un service système, pas dans une tâche et
pas dans le HAL. Ce service est l'unique propriétaire du montage, du cache de secteur et du fichier
ouvert. Les producteurs utilisent une API de service étroite, par exemple une écriture de record borné,
une demande de `sync` et un état ; le service passe par un syscall pour les accès bloc. Ils ne reçoivent ni pointeur de cache ni accès bloc brut.

Cette organisation suit le chemin existant :

```text
tâches / producteurs -> API service journal -> service stockage -> syscall bloc -> HAL public
                                                           -> SD SPI -> SPI MCU
```

Le service doit sérialiser toutes les opérations. Un appel producteur ne doit jamais conserver un
pointeur fourni par l'appelant après son retour. Une petite file copie donc les records, applique une
politique explicite quand elle est pleine (`drop newest`, compteur de pertes), puis le service écrit
hors du chemin critique. Pour rester déterministe, aucune opération FAT/SD ne doit se faire en ISR et
aucune attente SPI ne doit garder les interruptions désactivées.

Une intégration immédiate de la carte comme second backend transparent de `tm_syslog()` est risquée :
le formateur actuel partage un état global et son verrou n'est pas sûr en cas de préemption ou de
réentrance. La V1 doit plutôt accepter des records déjà bornés via une API dédiée, ou corriger d'abord
le contrat de logging avec destinations, back-pressure et résultat de livraison.

## Sous-ensemble FAT16 recommandé

### Format accepté

- MBR avec une partition FAT16, ou volume FAT16 « superfloppy », choisi par configuration ;
- secteur logique strictement égal à 512 octets ;
- paramètres du BPB validés avec arithmétique 32 bits et contrôles de débordement ;
- exactement deux FAT ou nombre lu et validé ;
- répertoire racine FAT16 de taille fixe ;
- noms courts 8.3 ASCII majuscules seulement ;
- rejet explicite de FAT12, FAT32, LFN, sous-répertoires et géométries non supportées.

Le type FAT ne doit pas être déduit de l'étiquette texte : il se détermine par le nombre de clusters.
Toutes les lectures multi-octets du disque sont little-endian et potentiellement non alignées ; il
faut les décoder octet par octet, jamais caster le buffer secteur vers une structure C.

### Stratégie d'écriture V1

La stratégie la plus sûre et la moins coûteuse est de demander que `LOG.TXT` ou `DATA.CSV` soit créé
et préalloué sur PC, dans une chaîne de clusters contigus. Au montage, TaskMate trouve l'entrée racine,
vérifie la chaîne FAT une fois, retrouve la position correspondant à la taille courante, puis ajoute
les octets. Cela évite l'allocateur de clusters pendant l'exploitation et rend la latence plus
prévisible.

À chaque `sync` :

1. compléter et écrire le secteur de données sale ;
2. attendre la fin réelle de programmation de la carte ;
3. mettre à jour la taille dans l'entrée de répertoire ;
4. écrire le secteur de répertoire et attendre à nouveau.

La taille publiée ne doit jamais dépasser les données déjà écrites. Une coupure peut perdre les
données depuis le dernier `sync`, mais ne devrait pas annoncer des octets non persistés. Il reste
possible que le secteur de répertoire soit endommagé par une coupure pendant sa programmation : FAT16
ne fournit ni journal ni atomicité.

Une variante qui crée le fichier et étend sa chaîne de clusters est possible en V2. Elle doit alors
mettre à jour les deux FAT et le répertoire dans un ordre documenté, détecter le volume plein et
reprendre un scan borné. Elle ajoute environ 3 à 7 jours, 1 à 3 Kio de flash, davantage de cas de
coupure et des latences moins prévisibles.

### Compatibilité PC

Le PC peut lire le fichier après un `sync`, un démontage logique et le retrait de la carte. Il ne doit
jamais accéder au même média simultanément : une carte SD SPI reliée au MCU n'est pas un périphérique
USB mass-storage. Pour une lecture sans extraction, il faudrait en plus une pile USB MSC et arbitrer
l'exclusivité du volume, ce qui est hors périmètre.

Le texte devrait être émis en ASCII/UTF-8 sans BOM, avec fins de ligne CRLF si la compatibilité avec
des outils Windows anciens est importante. CSV est préférable pour des données structurées. Une
ligne incomplète après coupure doit être détectable ; numéro de séquence et CRC par record sont utiles
pour les logs critiques.

## Estimation des ressources

### SRAM

Budget pour une implémentation sans allocation dynamique :

| Poste | Minimum | Confortable | Commentaire |
|---|---:|---:|---|
| Cache secteur partagé | 512 o | 512 o | un seul buffer pour boot, FAT, racine et données |
| État SD/SPI | 24 o | 64 o | réponses, timeouts, LBA, statut |
| État FAT/fichier | 64 o | 160 o | BPB décodé, clusters, entrée racine, offsets |
| File de records | 64 o | 256 o | politique de perte obligatoire |
| Pile du service | 256 o | 512 o | à mesurer par canari, fonctions non récursives |
| **Total** | **920 o** | **1 504 o** | hors éventuel buffer de formatage déjà existant |

Le cache unique est un impératif : deux caches sectoriels coûteraient déjà 1 Kio. Les structures
« packed » mappées sur disque sont déconseillées ; elles économisent peu et créent des accès non
alignés/aliasing fragiles. Conserver les constantes et messages d'erreur en `PROGMEM` évite de payer
leur copie en SRAM sur l'architecture Harvard.

### Flash

| Composant | Fourchette |
|---|---:|
| SPI ATmega2560 + HAL public | 0,5 à 1,5 Kio |
| Protocole SD bloc, timeouts et erreurs | 1,5 à 3 Kio |
| Montage et append FAT16 réduit | 3 à 6 Kio |
| Syscalls, service, file et intégration logs | 1 à 2 Kio |
| **Total V1** | **6 à 12 Kio** |

Ces valeurs supposent `-Os`, LTO, aucune écriture générique de répertoire et pas de bibliothèque FAT
complète. Les messages de diagnostic, tables CRC et chemins optionnels doivent être gardés par options
de compilation afin que `--gc-sections` puisse les retirer. Une mesure `.text + .data` sur le binaire
lié est requise avant validation.

### CPU et latence

À 4 MHz, transmettre 512 octets prend au minimum `512 × 8 / 4 MHz = 1,024 ms`, hors commande, token
et attente carte. Une lecture sera typiquement de l'ordre de quelques millisecondes. Une écriture peut
prendre plusieurs millisecondes et présenter des pointes beaucoup plus longues dues à l'effacement et
au nivellement d'usure internes ; ces pointes doivent être mesurées sur la référence exacte de carte.

Avec un SPI interrogé, le CPU est occupé presque entièrement durant les octets transmis. À un débit de
logs de 1 Kio/s, deux secteurs de données par seconde représentent environ 2 ms de temps au fil, puis
les attentes de carte et les écritures de métadonnées. Une moyenne de 1 à 5 % est plausible si les
attentes font des yields courts ; elle ne constitue pas une borne temps réel. Faire un `sync` à chaque
ligne peut multiplier fortement la charge et l'usure. Un `sync` périodique (par exemple chaque seconde
ou chaque N secteurs) échange une fenêtre de perte connue contre de meilleures performances.

Le scheduler coopératif permet de céder pendant une attente « card busy », mais pas au milieu d'une
transaction dont le chip-select et le bus doivent rester possédés. Un verrou de bus borné sera requis
si d'autres périphériques SPI sont ajoutés. Les timeouts doivent progresser même si le scheduler tourne
et être testés lors d'une carte absente ou retirée.

## Risques et mesures de maîtrise

| Risque | Impact | Mesure proposée |
|---|---|---|
| Coupure pendant données/métadonnées | perte récente ou volume à réparer | données avant taille, `sync` borné, préallocation, tests de coupure |
| Latence SD non déterministe | échéances RTOS manquées | service basse priorité, file bornée, mesure P99/max, compteur de pertes |
| SRAM/pile insuffisante | corruption silencieuse | cache unique, canaris, marge minimale documentée, pas de récursion |
| Retrait pendant écriture | erreurs ou corruption | détection/timeout, passage en état erreur, remontée au producteur |
| Usure par mises à jour fréquentes | durée de vie réduite | agrégation par secteur et fréquence de `sync` configurable |
| Carte ou format incompatible | mauvais calcul de LBA | validation stricte BPB/cluster, rejet en lecture seule avant toute écriture |
| Deux propriétaires du média | corruption | service propriétaire unique, jamais PC et MCU en parallèle |
| Régression architecturale | couplage service/HAL | contrats interfaces, syscall bloc, allowlists et contrôles de build |
| Temps bloqué interruptions coupées | perte ticks/USART | aucune transaction en section atomique globale, ISR courtes |

L'utilisation d'une carte « high endurance » aide l'endurance, mais ne transforme pas la SD en média à
latence déterministe. Le condensateur de maintien, la détection de chute d'alimentation et une commande
de `sync` d'urgence peuvent réduire le risque, sans garantir une programmation si la carte a déjà
commencé une opération interne longue.

## Plan de réalisation et complexité

| Lot | Contenu | Estimation |
|---|---|---:|
| 0 — preuve matérielle | câblage, SPI, init SD, lecture/écriture LBA, histogramme latence | 3–5 j |
| 1 — HAL robuste | timeouts, erreurs, SDHC/SDSC, driver control, build/autoCode | 3–5 j |
| 2 — FAT16 lecture | BPB, détection FAT16, racine 8.3, validation chaîne contiguë | 3–4 j |
| 3 — append/sync | cache unique, écriture, taille, volume plein, reprise après reset | 3–5 j |
| 4 — service/API logs | propriétaire, file bornée, back-pressure, compteurs, format | 3–4 j |
| 5 — validation | images disque, tests négatifs, coupures, PC, piles et charge | 3–5 j |
| **Total V1** | lots partiellement parallélisables après la preuve | **18–28 j** |

La fourchette résumée de 15–25 jours de développement exclut la campagne matérielle finale ; le total
incluant validation est donc 18–28 jours. Ajouter la création/allocation de fichier, les répertoires,
les noms longs, plusieurs fichiers ouverts ou une récupération transactionnelle ferait monter le coût
vers 30–50 jours et augmenterait nettement la surface de test.

## Critères d'acceptation proposés

1. Monter et ajouter des données à au moins trois images FAT16 couvrant les tailles de cluster admises.
2. Rejeter sans écriture les BPB invalides, FAT12/FAT32, chaînes non contiguës et secteurs non 512 o.
3. Lire le fichier sans réparation sous Windows, Linux et macOS après `sync` puis retrait.
4. Injecter une coupure avant/après chaque écriture de secteur ; le volume reste montable ou l'erreur
   est détectée et documentée, sans écriture hors partition.
5. Tester carte absente, retrait, write-protect si disponible, volume plein et timeout « busy ».
6. Mesurer SRAM/flash sur ELF/map, marge de pile par canari et latences min/moyenne/P99/max sur carte.
7. Prouver que ticks, contexte, USART RX et autres échéances restent acceptables pendant les pointes.
8. Vérifier autoCode, dépendances et allowlists sur la cible par défaut et tout autre HWT affecté.

## Découpage recommandé

Commencer par le lot 0 sans FAT : écrire puis relire des motifs connus dans une zone de test d'une
carte sacrifiable. La décision de poursuivre dépend de trois seuils à fixer avant mesure : latence
maximale tolérée, débit de logs soutenu et fenêtre de perte admise. Ensuite seulement implémenter le
lecteur FAT16 et le fichier préalloué. Cette séquence réduit le risque majeur — le comportement réel de
la carte — avant d'investir dans le format de fichier.

Ne pas commencer par porter une bibliothèque FAT complète. Une bibliothèque éprouvée peut réduire les
bugs de format, mais son intégration doit tout de même respecter l'absence d'allocation dynamique, les
frontières HAL/syscall/service, la licence, les accès AVR et la politique de concurrence. Elle doit
être comparée au périmètre réduit avec des mesures réelles de flash, SRAM et pile.
