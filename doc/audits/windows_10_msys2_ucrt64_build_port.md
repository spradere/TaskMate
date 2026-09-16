# Audit de portage du build vers Windows 10, MSYS2 et UCRT64

Date : 16 septembre 2026  
Branche auditée : `test`  
Révision auditée : `d0fabace2babc6fb61828d30b737ea0125c6639e`  
Poste observé : Windows 10 Pro 22H2, build 19045, Cygwin 3.6.6, `bmake` 20240314

## Objet et périmètre

Cet audit évalue la faisabilité technique et le mode opératoire d'un portage du système de build de
TaskMate vers Windows 10 avec MSYS2 et son environnement UCRT64. Il couvre l'orchestration BSD
`bmake`, autoCode, les scripts shell et AWK, la compilation AVR, les tests hôte, les outils de qualité,
le flashage d'une Arduino Mega et les utilitaires annexes.

Le code du RTOS et son architecture embarquée ne nécessitent pas de portage vers Windows : seul l'hôte
de construction change. La cible reste `avr8 / atmega2560 / Arduino Mega` et doit conserver les mêmes
options de compilation, contrôles de frontières, données générées et contraintes déterministes.

L'audit repose sur la lecture de `doc/architecture/build.md`, `doc/architecture/autoCode.md`, des règles
du projet, du `Makefile`, de `mk/*.mk`, des fragments AVR, des scripts et des tests. Il n'a pas installé
MSYS2 ni modifié le build.

## Verdict

**Le portage vers MSYS2/UCRT64 est faisable, avec un risque technique moyen et un effort estimé à trois
à six jours de développement et de validation hors matériel.** Il ne fonctionne toutefois pas sans
adaptation dans l'état actuel du dépôt.

La solution recommandée est hybride :

- le shell, AWK et les utilitaires POSIX proviennent du sous-système MSYS ;
- Clang, la toolchain AVR et `avrdude` sont des exécutables Windows UCRT64 natifs ;
- BSD `bmake` est conservé et provisionné comme outil épinglé du projet ;
- les cibles strictement FreeBSD, notamment `backup`, sont isolées du build normal ;
- une commande de diagnostic vérifie les outils requis par la cible demandée, et non tous les
  utilitaires possibles.

MSYS2 recommande UCRT64 et construit son `PATH` sous la forme `/ucrt64/bin:/usr/bin` : les outils
Windows natifs sont donc prioritaires, tout en gardant les commandes MSYS nécessaires aux recettes
POSIX. C'est précisément le modèle adapté à TaskMate. Il faut cependant comprendre que MSYS2 ne
supprime pas toute couche d'émulation : ses outils `/usr/bin` reposent encore sur un runtime dérivé de
Cygwin. Le gain de vitesse par rapport à Cygwin doit être mesuré, pas présumé.

Un obstacle de distribution subsiste : le dépôt MSYS2 ne contient pas de paquet `bmake` au jour de
l'audit. GNU Make ne sait pas interpréter sans réécriture les `.include`, `.if`, `.for`, `.WAIT`, les
modificateurs `:T`, `:ts`, les affectations `!=` et les transformations de suffixes utilisées ici.
Le bootstrap de pkgsrc sous MSYS2 est documenté et installe `bmake`; il constitue une voie de preuve de
concept acceptable. Pour une utilisation pérenne, TaskMate devrait produire un paquet ou une archive
interne de `bmake`, avec version et somme de contrôle figées.

## Architecture actuelle du build

Le build complet suit aujourd'hui cette chaîne :

1. `bmake` lit la composition cible → carte → MCU → architecture ;
2. une cible `.BEGIN` vérifie une liste globale de programmes et crée les répertoires de build ;
3. les fichiers sont découverts par `find` et des listes dynamiques sont calculées par le shell ;
4. autoCode est compilé pour l'hôte avec Clang, puis génère les fragments contractuels ;
5. les contrôles d'en-têtes et d'architecture sont exécutés en AWK ;
6. le firmware est compilé et lié par `avr-gcc` ;
7. `avr-size`, AWK et CLOC produisent le bilan ;
8. `avrdude` flashe éventuellement la carte.

Cette organisation est portable vers un environnement POSIX Windows. Les formats générés et le code
embarqué n'emploient aucune API Windows. Les principaux risques sont dans la disponibilité des outils,
les noms de fichiers Windows, les chemins et les cibles périphériques.

## Compatibilité des composants

| Composant | État sous MSYS2/UCRT64 | Action |
| --- | --- | --- |
| BSD `bmake` | Bloquant : absent du dépôt MSYS2 officiel | Bootstrap pkgsrc pour le prototype, puis paquet interne épinglé |
| `sh`, AWK, `find`, `grep`, `sed`, Coreutils | Disponibles dans le dépôt MSYS | Installer explicitement les paquets, conserver `/usr/bin` après `/ucrt64/bin` |
| Clang hôte | Disponible en UCRT64 | Installer `mingw-w64-ucrt-x86_64-clang` |
| `clang-format`, `clang-tidy` | Disponibles, sans suffixe de version garanti | Utiliser des variables d'outil et les noms non versionnés |
| AVR GCC, Binutils et avr-libc | Groupe UCRT64 officiel complet | Installer `mingw-w64-ucrt-x86_64-avr-toolchain` |
| `avrdude` | Paquet UCRT64 officiel | Installer séparément ; configurer un port `COMn` |
| CLOC, Ctags, Doxygen, Cppcheck, rsync | Disponibles, mais non requis par tout build | Vérifier seulement dans les cibles qui les emploient |
| autoCode | C portable compilé pour l'hôte | Tester les renommages, fins de ligne et suffixes `.exe` |
| ASan/UBSan hôte | À qualifier avec la version Clang retenue | Cible facultative tant que le runtime n'est pas validé |
| `backup` | Incompatible : périphérique et montage FreeBSD | Désactiver sous Windows ou concevoir une cible Windows séparée et sûre |
| Upload Arduino | Faisable avec `avrdude.exe` | Rendre le port configurable (`COM3`, etc.) et tester sur matériel |

Le groupe UCRT64 AVR officiel contient bien `avr-binutils`, `avr-gcc` et `avr-libc`; l'ATmega2560 est
présent dans les spécifications livrées par GCC. Le paquet `avrdude` fournit un exécutable natif sous
`/ucrt64/bin`.

## Écarts bloquants ou importants dans le dépôt

### 1. Nom de journal interdit sous Windows

`mk/sources.mk:35` fabrique `VAL_DATE_TIME` avec `%H:%M:%S`, puis ce texte entre dans le nom du journal
autoCode. Le caractère `:` est interdit dans un nom de fichier Win32. Il faut employer par exemple
`%Y_%m_%d_%H-%M-%S`. Cette correction est portable et ne change aucune donnée embarquée.

### 2. Dépendances globales trop larges

`conf/programs-list.conf` exige notamment CLOC, Ctags, `mount`, `umount` et rsync avant le build normal,
alors que ces outils concernent des fonctions optionnelles. Cette conception explique une partie des
« programmes manquants » observés sous Cygwin et reproduirait le problème sous MSYS2.

Il faut séparer au minimum :

- les outils nécessaires à l'analyse du Makefile et au firmware ;
- les outils des tests hôte ;
- les outils de qualité et documentation ;
- les outils d'édition ;
- les outils de sauvegarde propres à la plateforme.

Le contrôle doit être un prérequis explicite des cibles concernées, pas un effet global de `.BEGIN`.
Le stamp doit inclure ou vérifier l'environnement actif, car un stamp ancien survit actuellement à un
changement de `PATH`.

### 3. Outils et chemins codés en dur

`mk/utils.mk` appelle `clang-format19` et `clang-tidy19`, tandis que le paquet UCRT64 fournit les noms
usuels `clang-format` et `clang-tidy`. Les commandes Cppcheck et Clang-Tidy contiennent aussi
`/root/code/TaskMate/TaskMate_current`, chemin étranger au checkout Windows.

Il faut définir des variables de rôle, par exemple `FORMAT`, `TIDY` et `CPPCHECK`, avec des valeurs par
défaut non versionnées, puis utiliser uniquement `${.CURDIR}` et `${PATH_SRCS}` pour les inclusions. La
cible documentée `clang_format` ne correspond en outre pas à la cible réelle `format`; un seul nom doit
être retenu ou un alias ajouté.

### 4. Contrat de chemins insuffisant

La découverte `find` est transformée en listes Make séparées par des espaces et de nombreuses recettes
développent ces listes sans guillemets. Les noms internes du dépôt sont simples, mais le chemin absolu
du checkout observé contient `PC HP`. Les chemins relatifs limitent le problème sans le supprimer,
notamment pour les dépendances produites par les compilateurs et les appels `bmake -C`.

Pour le premier port, il est recommandé de travailler sous un chemin ASCII sans espace, par exemple
`C:\work\TaskMate_current`, et d'en faire un contrat explicite. Étendre immédiatement le build à tous
les noms Win32/POSIX serait disproportionné.

### 5. Fins de ligne et exécution des scripts

Les scripts `#!/bin/sh` doivent rester en LF. Une conversion Git en CRLF peut rendre le shebang ou des
comparaisons de fichiers invalides. Le dépôt devrait fixer les fins de ligne de `*.sh`, `*.awk`, `*.mk`,
`Makefile`, `*.rc`, `*.err`, `*.gpio` et `*.list` dans `.gitattributes`, sans dépendre du réglage global
`core.autocrlf` de chaque poste.

Le comportement du binaire autoCode natif doit aussi être qualifié : suffixe `.exe`, ouverture en mode
texte, fins de ligne produites, `rename()` d'un fichier existant et stabilité de
`compare-and-replace`. Les tests existants couvrent bien la logique fonctionnelle et doivent devenir
la preuve de compatibilité Windows.

### 6. Cibles FreeBSD et périphériques

`PATH_USBKEY=/media/usbkey`, `FILE_USBDEV=/dev/da0s1` et `mount -t msdosfs` sont spécifiques à
FreeBSD. La cible `backup` utilise en plus `rsync --delete`; elle ne doit jamais être adaptée par une
simple substitution de chemin. Elle doit être rendue indisponible sous Windows avec un diagnostic
clair, ou remplacée par une implémentation Windows séparée avec validation canonique du volume,
sentinelle et simulation préalable.

Le port série `/dev/ttyU0` est également spécifique. `VAL_PROGRAMMER_PORT` doit être surchargeable, par
exemple `bmake upload VAL_PROGRAMMER_PORT=COM3`. Le flashage doit rester une étape matérielle explicite,
hors validation automatique initiale.

### 7. Version des outils et reproductibilité

Le paquet UCRT64 AVR évolue indépendamment de l'environnement historique. Une nouvelle version de GCC
peut modifier diagnostics, taille, LTO et binaire final sans changement source. Le portage doit donc
enregistrer au minimum les sorties de `bmake`, `clang`, `avr-gcc`, `avr-ld`, `avr-libc` et `avrdude`, et
figer un instantané MSYS2 ou une liste de versions qualifiées pour les releases.

Comparer uniquement les fichiers HEX entre Cygwin et MSYS2 n'est pas une preuve suffisante si les
versions de compilateur diffèrent. La qualification doit comparer à version égale, puis examiner les
écarts de taille, sections, symboles, désassemblage et comportement sur la carte.

## Mode opératoire recommandé

### Phase 0 — conserver un retour arrière

1. Conserver l'installation Cygwin actuelle jusqu'à validation complète de MSYS2.
2. Capturer ses versions d'outils, un build propre, les rapports mémoire, le HEX et le journal
   autoCode de référence.
3. Ne pas partager un même répertoire `build/` entre une exécution Cygwin et une exécution MSYS2 ;
   effectuer les comparaisons dans deux checkouts ou deux worktrees distincts.

L'installation Cygwin observée est incomplète : avec un `PATH` POSIX explicite, il manque au minimum
AWK, Clang, Ctags, Findutils, Git, rsync, Sed et toute la toolchain AVR, dont `avrdude`. Le paquet
`bmake` est en revanche bien installé. Cela montre que l'échec actuel vient largement du provisionnement
et du contrôle global des outils, pas d'une impossibilité fondamentale de Cygwin.

### Phase 1 — installer MSYS2/UCRT64

Sur Windows 10 22H2, installer MSYS2 dans son chemin standard `C:\msys64`, mettre à jour le système,
fermer/réouvrir le terminal si demandé, puis répéter la mise à jour :

```sh
pacman -Syu
pacman -Syu
```

Toujours lancer le profil **UCRT64**, et vérifier :

```sh
test "${MSYSTEM}" = UCRT64
printf '%s\n' "${PATH}"
```

Le début du `PATH` attendu est `/ucrt64/bin:/usr/bin`. Ne pas ajouter globalement les répertoires de
Cygwin au `PATH` MSYS2 : mélanger les DLL, shells et conventions de chemins rendrait les diagnostics
non reproductibles.

### Phase 2 — installer les paquets

Jeu initial proposé pour le prototype :

```sh
pacman -S --needed \
	base-devel bash coreutils diffutils findutils gawk grep sed git rsync cloc ctags doxygen \
	mingw-w64-ucrt-x86_64-clang \
	mingw-w64-ucrt-x86_64-clang-tools-extra \
	mingw-w64-ucrt-x86_64-cppcheck \
	mingw-w64-ucrt-x86_64-avr-toolchain \
	mingw-w64-ucrt-x86_64-avrdude
```

Le manifeste définitif devra être obtenu avec `pacman -Q` après qualification et archivé avec le
rapport de build. Les éditeurs et outils de sauvegarde ne doivent pas entrer dans le socle minimal.

### Phase 3 — provisionner BSD `bmake`

Pour la preuve de concept, utiliser le bootstrap non privilégié de pkgsrc dans un emplacement sans
espace. Le guide NetBSD précise que ce bootstrap installe `bmake`, et un retour spécifique à
Windows 10/11 avec MSYS2 UCRT64 confirme ce chemin.

Exemple de principe :

```sh
git clone --depth 1 https://github.com/NetBSD/pkgsrc.git /opt/pkgsrc
cd /opt/pkgsrc/bootstrap
./bootstrap --unprivileged --prefix=/opt/taskmate-pkg
export PATH=/opt/taskmate-pkg/bin:/ucrt64/bin:/usr/bin
bmake -V MAKE_VERSION
```

Ce clone courant ne convient pas à une chaîne de release durable. Après le prototype :

1. choisir une version de `bmake` validée ;
2. conserver les sources exactes et leur licence ;
3. construire un paquet MSYS2 interne ou une archive d'outils ;
4. publier la somme SHA-256 et la procédure reproductible ;
5. tester explicitement `.WAIT`, `.MAKE.EXPAND_VARIABLES`, `:T`, `:ts`, `!=`, `.for` et `.include` ;
6. refuser une version inconnue dans la commande de diagnostic.

### Phase 4 — appliquer les adaptations minimales du dépôt

Ordre conseillé :

1. remplacer les deux-points du nom de journal ;
2. introduire les variables d'outils hôte et supprimer les chemins `/root/...` ;
3. séparer les manifestes de programmes par cible et retirer la vérification de `.BEGIN` ;
4. déclarer les fins de ligne Git ;
5. rendre le port de programmation surchargeable ;
6. déclarer `backup` non pris en charge sur Windows ;
7. ajouter une cible `doctor` non destructive affichant environnement, chemins et versions ;
8. enregistrer un manifeste de versions dans `build/`.

Ces changements doivent rester dans le système de build. Aucun `#if Windows` ne doit être introduit
dans le code firmware ou dans autoCode lorsque le C standard suffit.

### Phase 5 — valider sans matériel

Depuis un checkout sans espace et avec `MSYSTEM=UCRT64` :

```sh
bmake doctor
bmake -V VAL_HW_STACK
bmake -V FILES_SRC
bmake test_build_system
bmake test_autoCode
bmake test_tm_string
bmake autoCode_alone
bmake
bmake clean
bmake
```

Critères d'acceptation :

- toutes les commandes retournent zéro et aucun outil Windows homonyme, tel `find.exe` de Windows,
  n'est sélectionné ;
- les tests autoCode ne laissent aucun `.tmp` et un second passage ne modifie pas les sorties ;
- les contrôles d'architecture et d'en-têtes bloquent bien sur une fixture invalide ;
- `clean` reste confiné sous `build/` avec les chemins MSYS ;
- le firmware lie pour ATmega2560, les sections et la RAM restent dans les limites ;
- un second build sans changement ne recompile ni autoCode ni le firmware inutilement ;
- le résultat est identique entre deux builds MSYS2 propres utilisant le même manifeste d'outils.

La cible sanitizer doit être qualifiée séparément. Son absence temporaire ne doit pas masquer l'échec
des tests ordinaires, mais elle doit rester obligatoire sur au moins une plateforme de CI supportée.

### Phase 6 — valider le matériel et les performances

1. Identifier le port Arduino dans le Gestionnaire de périphériques.
2. Exécuter `avrdude -v` et sauvegarder sa version/configuration.
3. Flasher explicitement avec `VAL_PROGRAMMER_PORT=COMn`.
4. Faire un test de démarrage, timer, ordonnanceur, GPIO et USART/SCLI sur la Mega 2560.
5. Comparer Cygwin et MSYS2 sur cinq builds propres et cinq builds incrémentaux, antivirus et machine
   identiques. Mesurer temps total, temps autoCode, compilation, lien et rapport CLOC.

Une amélioration de performance ne doit pas être obtenue en désactivant globalement l'antivirus sur le
répertoire de développement. Si l'antivirus domine les mesures, toute exclusion doit faire l'objet
d'une décision de sécurité locale explicite et limitée.

## Autres voies possibles

| Voie | Faisabilité | Avantages | Limites | Avis |
| --- | --- | --- | --- | --- |
| MSYS2/UCRT64 hybride | Bonne après adaptations | Paquets récents, AVR et avrdude natifs, intégration Windows directe | `bmake` à provisionner ; commandes POSIX toujours sous runtime MSYS | **Choix recommandé pour le pilote** |
| Cygwin64 durci | Très bonne, effort minimal | `bmake` officiellement empaqueté ; sémantique POSIX déjà connue | Lenteur constatée, installation actuelle incomplète, toolchain à installer | Excellent filet de sécurité et solution court terme |
| WSL2 Linux | Très bonne pour build/test | Paquets Linux complets, environnement CI naturel, bonnes performances si le dépôt est dans le FS Linux | USB non natif, `usbipd-win` nécessaire ; accès `/mnt/c` plus coûteux ; couche VM | **Meilleure alternative si le bootstrap `bmake` MSYS2 est refusé** |
| Conteneur Linux sous WSL2/Docker | Bonne pour build reproductible | Image versionnée, isolation forte des dépendances | USB et flashage pénibles, volume Windows potentiellement lent, complexité supplémentaire | Bon candidat CI, moins bon poste embarqué interactif |
| VM Linux/FreeBSD complète | Bonne | Proche de l'environnement Unix de référence, USB pass-through possible | Coût d'administration, stockage et démarrage, intégration éditeur moindre | Solution de repli robuste mais lourde |
| Réécriture CMake + Ninja native | Faisable à moyen terme | Très bonne portabilité et vitesse Windows native, écosystème IDE/CI large | Réécriture du graphe, double maintenance transitoire, risque sur autoCode et contrôles | Étude future, pas préalable au port MSYS2 |
| GNU Make/Git Bash/Scoop seuls | Faible sans réécriture | Installation légère | GNU Make incompatible avec les Makefiles BSD ; outils incomplets et versions dispersées | Non recommandé |

### Cygwin64 peut être réparé immédiatement

Cygwin publie officiellement un paquet `bmake`. Les autres manques constatés peuvent être installés par
`setup-x86_64.exe -P ...`. Une action rapide consiste donc à produire une liste de paquets Cygwin
versionnée et à scinder les contrôles de programmes. Cela ne résout pas la lenteur intrinsèque observée,
mais fournit une référence fiable pendant le pilote MSYS2.

Il ne faut pas copier `bmake.exe` depuis Cygwin vers MSYS2 : le binaire dépend du runtime Cygwin et
relancerait des outils avec ses propres conventions. Deux environnements complets et séparés sont plus
sûrs qu'un mélange de leurs exécutables.

### WSL2 est la meilleure voie de repli

Le poste observé, build Windows 19045, satisfait le minimum WSL moderne. Le dépôt doit être cloné dans
le système de fichiers Linux, pas sous `/mnt/c`, pour éviter le coût des accès inter-systèmes. La
toolchain AVR, Clang, les tests et `bmake` y sont simples à provisionner.

Le flashage requiert toutefois `usbipd-win`, car l'USB n'est pas présenté nativement à WSL. Microsoft
documente explicitement ce mécanisme pour des scénarios tels que le flashage Arduino. Sous Windows 10,
il faut valider la version Store de WSL et le noyau requis avant d'en faire une solution opérateur.

### CMake/Ninja ne doit pas être confondu avec le portage hôte

Une conversion vers CMake pourrait supprimer à terme la dépendance à `bmake` et la majorité des scripts
de découverte. Elle doit néanmoins reproduire exactement : composition matérielle, ordre de démarrage,
génération autoCode, dépendances dynamiques, contrôles de frontières, fichiers de dépendances AVR,
rapports mémoire, tests négatifs et règles de nettoyage.

C'est un projet de build distinct, avec coexistence et tests différentiels nécessaires. Le faire en
même temps que le changement d'environnement multiplierait les causes possibles d'écart. Il est plus
sûr de qualifier d'abord le build BSD existant sous MSYS2 ou WSL2, puis de décider sur mesures si une
réécriture native est rentable.

## Risques et décisions à prendre

| Risque | Niveau | Réduction recommandée |
| --- | --- | --- |
| `bmake` non fourni par MSYS2 | Haut | Paquet interne épinglé, checksum et test de dialecte |
| Noms/path/fins de ligne Windows | Haut avant correction | Timestamp sans `:`, checkout sans espace, `.gitattributes`, corpus autoCode |
| Dérive de version AVR | Haut pour une release embarquée | Manifeste et qualification d'une version précise |
| Gain de vitesse inférieur aux attentes | Moyen | Benchmark par phase avant abandon de Cygwin |
| Mélange MSYS/Cygwin/Windows dans le `PATH` | Haut | Environnements isolés et cible `doctor` |
| Flashage série différent | Moyen | Port configurable et essai matériel explicite |
| Cible `backup` destructive/incompatible | Haut | La rendre indisponible sous Windows |
| Windows 10 hors support standard | Haut en sécurité | ESU ou migration Windows 11, même si le build reste techniquement possible |

Windows 10 22H2 a atteint sa fin de support standard le 14 octobre 2025. MSYS2 reste techniquement
compatible avec cette version, mais une chaîne connectée à Internet qui télécharge paquets et sources
ne devrait pas être pérennisée sur un hôte sans ESU. Cette question de cycle de vie est indépendante de
la réussite du portage, mais elle doit entrer dans l'acceptation opérationnelle.

## Estimation

| Lot | Charge indicative |
| --- | ---: |
| Installation MSYS2, paquets et prototype `bmake` | 0,5 à 1 jour |
| Corrections minimales du build et cible `doctor` | 1 à 2 jours |
| Tests hôte, déterminisme et comparaison Cygwin | 1 à 2 jours |
| Paquet interne/documentation/manifestes | 0,5 à 1 jour |
| Flashage et recette Arduino Mega | 0,5 jour |

L'estimation exclut une réécriture CMake/Ninja, un nouveau système de sauvegarde Windows et la
résolution d'éventuels défauts fonctionnels préexistants révélés par les tests.

## Décision recommandée

Lancer un pilote MSYS2/UCRT64 limité à une branche de portage, sans supprimer Cygwin. Le jalon de
décision doit être pris après les phases 1 à 5 : si le firmware et tous les tests hôte passent avec un
`bmake` empaqueté et si le benchmark apporte un gain utile, MSYS2 devient l'environnement Windows de
référence. Si le maintien d'un paquet `bmake` est jugé trop coûteux ou si le gain est faible, adopter
WSL2 pour le build et conserver un outil Windows natif séparé pour le flashage.

À très court terme, corriger le provisionnement Cygwin reste utile : il donne une base de comparaison
complète et peut débloquer le projet avant la fin du pilote.

## Sources externes

- [Environnements MSYS2 et recommandation UCRT64](https://www.msys2.org/docs/environments/)
- [Gestion des paquets MSYS2](https://www.msys2.org/docs/package-management/)
- [Paquets du dépôt MSYS et absence actuelle de bmake](https://packages.msys2.org/packages/?repo=msys)
- [Groupe officiel AVR UCRT64](https://packages.msys2.org/groups/mingw-w64-ucrt-x86_64-avr-toolchain)
- [Paquet officiel avr-gcc UCRT64](https://packages.msys2.org/packages/mingw-w64-ucrt-x86_64-avr-gcc)
- [Paquet officiel avrdude UCRT64](https://packages.msys2.org/packages/mingw-w64-ucrt-x86_64-avrdude)
- [Clang-Tidy et outils Clang UCRT64](https://packages.msys2.org/packages/mingw-w64-ucrt-x86_64-clang-tools-extra)
- [Bootstrap pkgsrc sur une plateforme non-NetBSD](https://www.netbsd.org/docs/pkgsrc/platforms.html)
- [Retour de bootstrap pkgsrc sous Windows et MSYS2 UCRT64](https://mail-index.netbsd.org/pkgsrc-users/2024/03/23/msg039236.html)
- [Paquet bmake officiel de Cygwin](https://cygwin.com/packages/summary/bmake.html)
- [Installation de WSL et prérequis Windows 10](https://learn.microsoft.com/en-us/windows/wsl/install)
- [Connexion USB à WSL avec usbipd-win](https://learn.microsoft.com/en-us/windows/wsl/connect-usb)
- [Cycle de vie de Windows 10](https://learn.microsoft.com/en-us/lifecycle/faq/windows)

## Validation réalisée pendant l'audit

- inventaire des dépendances directes dans Make, shell, AWK et les fragments AVR ;
- contrôle des paquets officiels MSYS2 disponibles au 16 septembre 2026 ;
- constat de l'absence de MSYS2 sur le poste, sans installation ni modification système ;
- lecture de la version Cygwin et de `bmake` installées ;
- exécution non destructive du vérificateur de programmes Cygwin avec un `PATH` POSIX explicite ;
- vérification du système Windows 10 build 19045 ;
- aucune génération autoCode, compilation firmware, suppression, sauvegarde ou écriture matérielle.

La validation complète de faisabilité exige encore le pilote décrit ci-dessus et un essai physique sur
Arduino Mega 2560.
