# TaskMate — guide pour les agents

## Objet du dépôt

TaskMate est un RTOS expérimental pour microcontrôleurs, écrit principalement en C bas niveau. La cible actuelle est la pile `avr8 / atmega2560 / Arduino Mega`, mais le projet est conçu pour rester portable. La priorité est la sûreté, la lisibilité, le déterminisme et la séparation explicite des responsabilités.

Avant toute modification substantielle, lire la documentation concernée dans `doc/architecture/` et `doc/rules/`.

Les documents de référence généraux sont :

- `doc/rules/style.md` : style C, fichiers, commentaires et code généré ;
- `doc/rules/interfaces.md` et `doc/rules/arch_boundary_enforcement.md` : sens des dépendances et frontières contrôlées ;
- `doc/rules/portability.md` : découpage architecture / MCU / carte ;
- `doc/rules/autoCode.md` : générateur de code et ses entrées/sorties ;
- `doc/rules/TaskMate_prefixes.md` : préfixes réservés et leur sémantique ;
- `doc/C_code_best_practices.md` : règles de lisibilité, robustesse et commentaires.

## Architecture et règles de dépendance

L'arborescence est intentionnelle. Préserver ses frontières :

```text
srcs/user/tasks, srcs/system/services
              -> srcs/system/sysCall
              -> srcs/hal
              -> srcs/interfaces
```

- `srcs/interfaces/` définit des contrats portables et neutres. Il ne doit dépendre ni de la HAL, ni de `sysCall`, ni des services, ni des tâches ; seules les définitions C minimales et les en-têtes communs y sont admis.
- `srcs/hal/arch/<arch>/` porte le code propre à l'architecture (contexte, interruptions, atomiques).
- `srcs/hal/mcu/<mcu>/` contient les pilotes qui manipulent les registres du microcontrôleur.
- `srcs/hal/board/<board>/` contient le câblage, l'initialisation et les choix de carte.
- La HAL peut utiliser les interfaces, jamais les couches supérieures. Les tâches ne touchent ni aux registres, ni directement aux pilotes HAL : elles passent par les API `sysCall` et les services.
- Les violations de frontières sont contrôlées au build par des listes d'autorisation d'en-têtes. Ne pas contourner ce mécanisme avec des inclusions indirectes, des macros ou des exceptions ad hoc.

Conserver le choix des sources spécifiques à la cible dans l'arborescence et le build. Éviter de disperser des `#if` spécifiques au matériel dans le code portable.

## C embarqué

- Employer les types à largeur fixe de `<stdint.h>` et `bool` de `<stdbool.h>` lorsque pertinents.
- Garder les états internes privés avec `static`, si il faut les exposer utiliser une fonction.
- Valider les entrées, bornes et pointeurs ; expliciter les conditions ambiguës.
- Nommer les unités (`timeout_ms`, etc.), documenter les contraintes de timing, d'ISR, de concurrence, l'ordre d'initialisation et les accès matériels non évidents.
- Éviter les nombres magiques : utiliser des constantes, enums ou macros nommées. Les macros de registres et de masquage de bits doivent parenthéser paramètres et résultat.
- Préférer des fonctions petites, ciblées et des tables de données aux branches complexes. Ne pas introduire d'allocation dynamique ou de dépendance lourde sans raison architecturale explicite.
- Préserver la taille de pile, l'usage mémoire et les sections critiques ; toute modification de contexte, d'interruptions, d'atomiques, d'ordonnanceur ou de génération est système-critique et demande une vérification renforcée.

## Nommage et style

`.clang-format` et `doc/rules/style.md` font autorité.

- Indentation par tabulations (largeur 4), accolades Allman, lignes de 100 colonnes au plus.
- Conserver l'espacement du projet : `if( condition )`, `for( ... )`, `while( ... )`.
- Étoile des pointeurs à droite du type : `const tm_string_t *name;`.
- Les `.c` incluent d'abord leur en-tête local correspondant, puis les en-têtes système et enfin ceux du projet ; laisser `clang-format` ordonner les blocs.
- Tous les nouveaux fichiers C, en-têtes, fragments Make et scripts AWK commencent par la bannière BSD-2-Clause de `doc/licence_header.txt`. Les `.c` et `.h` ajoutent ensuite un bloc Doxygen `@file` / `@brief`.
- Les gardes d'en-tête sont en majuscules et basées sur le chemin (`SYSCORE_MODULES_H`), avec un commentaire sur le `#endif`.
- Types en `snake_case_t`, constantes/enums en `UPPER_SNAKE_CASE`, données locales statiques en `snake_case` descriptif.
- N'ajouter un préfixe que s'il exprime une frontière réelle. Les préfixes établis incluent `tm_` (identité TaskMate), `sc_` (syscall), `gpio_` (GPIO logique), `err_` (erreurs), `mod_` (base des modules) et `rl_` (run levels). Les API publiques emploient le lower camel case après le préfixe.
- Les commentaires expliquent le *pourquoi*, les contraintes ou le matériel ; ils ne paraphrasent pas le code.

## autoCode et fichiers générés

`srcs/autoCode/` est l'outil de génération ; les zones entre balises `autoCode` et les sections `clang-format off` associées sont générées. Ne jamais les modifier à la main : une régénération les écraserait.

Lorsqu'une configuration de modules, un `*.rc`, un `*.err`, des tags ou `autoCode` change :

1. modifier la source de vérité ;
2. lancer la génération/build ;
3. contrôler le journal autoCode et le diff des fichiers produits ;
4. vérifier que les fichiers générés restent cohérents avec la configuration.

Un changement dans autoCode peut produire un firmware qui compile mais se comporte incorrectement : le traiter comme une modification critique.

## Build : BSD bmake

Le système de build utilise la syntaxe BSD `bmake`, et non GNU Make. Utiliser `bmake` (ou la commande `make` seulement si elle pointe réellement vers BSD make) depuis la racine.

```sh
bmake                       # build de la cible HWT par défaut (test1)
bmake HWT=<target>          # sélection d'une cible utilisateur
bmake clean                 # nettoie les artefacts de la cible courante
bmake autoCode_alone        # régénère autoCode et affiche son dernier journal
bmake clang_format          # formate les sources connues du build
bmake tidy_autoCode         # analyse statique du générateur
bmake cppcheck              # analyse statique du projet
bmake help                  # cibles utilitaires disponibles
bmake doc                   # documentation Doxygen
```

Le `Makefile` orchestre les fragments `mk/*.mk`. La cible est validée puis le pipeline exécute la génération autoCode, les contrôles de dépendances/frontières, la compilation et l'édition de liens. Les artefacts sont sous `build/`; ne pas les ajouter au code source ni modifier les sorties de génération pour contourner une erreur.

Les variables de make suivent les préfixes définis dans `doc/rules/make_variable_prefixes.md` (`PATH_`, `PATHS_`, `FILE_`, `FILES_`, `VAL_`, `OPT_`, `COLOUR_`, `CFLAGS_`). Employer l'expansion BSD `${VAR}` de façon cohérente.

## Vérification attendue

Adapter les vérifications au risque : les modifications touchant à la HAL, aux interruptions, à l'ordonnanceur, au build ou aux frontières nécessitent un build complet et des contrôles statiques pertinents.

Ne pas modifier des fichiers hors du périmètre demandé, ne pas écraser des changements utilisateur existants, et signaler clairement les limites de validation matérielle lorsque la carte n'a pas été testée.
