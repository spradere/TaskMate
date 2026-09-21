# Audit des performances de la console ncurses

## Objet et périmètre

Cet audit analyse la lecture, le traitement et l'affichage de l'USART simulé sur la cible
`ucontext / pc / freebsd`. Il couvre `pc_console.c`, son appelant USART, l'affichage GPIO et la
consommation par SCLI. Il correspond au commit `9b396a9` et ne modifie aucun comportement.

La cible AVR n'utilise pas ncurses. Les recommandations concernent donc la fidélité et la
réactivité du simulateur hôte, sans introduire ce mécanisme dans les couches portables.

## Méthode et limites

L'analyse statique a suivi les chemins suivants :

```text
SCLI -> sc_usartRead() -> hal_usartRead() -> pc_consoleReadByte() -> wgetch()
tmLibc -> sc_consoleFlush() -> hal_usartSendTXBuffer() -> waddch() / wrefresh()
GPIO -> pc_consoleLedWrite() -> mvwprintw() / wrefresh()
```

Le code n'offre ni compteurs ni banc de performance ncurses. L'environnement de l'audit est Linux,
alors que l'implémentation emploie les signaux et `sigreturn()` de FreeBSD. Les priorités ci-dessous
reposent donc sur le nombre d'appels et leurs sections critiques, pas sur des temps présentés comme
mesurés. Une campagne FreeBSD sous pseudo-terminal est requise avant et après toute correction.

## Fonctionnement actuel

- ncurses est initialisé une fois et six fenêtres sont conservées statiquement.
- La saisie est non bloquante. Un appel vide à `wgetch()` termine le drainage des touches prêtes.
- Chaque caractère imprimable ou effacement redessine toute la ligne avec `werase()`, `waddnstr()`
  puis `wrefresh()`.
- Une ligne validée est copiée dans un second tampon de 64 octets. SCLI la relit ensuite octet par
  octet, avec une paire de masquage/restauration des signaux pour chaque octet.
- L'émission accumule jusqu'à 256 octets dans l'USART. Le vidage appelle `waddch()` par octet, puis
  un seul `wrefresh()`. tmLibc provoque normalement ce vidage à chaque fin de ligne.
- Chaque écriture GPIO reformate et rafraîchit immédiatement sa ligne, même si sa valeur est
  inchangée. Elle interroge aussi le clavier.

## Constats classés

### P1 - ncurses s'exécute dans des sections critiques longues

`hal_usartRead()` masque les signaux avant d'appeler la lecture ncurses. Le premier appel peut
drainer toutes les touches disponibles et rafraîchir l'écran plusieurs fois. L'émission garde aussi
les signaux masqués pendant toute la boucle de 256 octets et son `wrefresh()`. Enfin, les mises à
jour GPIO englobent `mvwprintw()` et `wrefresh()` dans une section critique.

Ces appels peuvent effectuer le calcul de différence d'écran et des écritures terminal dont la durée
n'est pas bornée par TaskMate. Ils retardent donc le signal d'ordonnancement de 1 ms et faussent le
temps simulé précisément lorsque les entrées/sorties sont actives. C'est le risque principal.

Il ne faut toutefois pas simplement retirer les sections critiques : les changements de contexte
par signal pourraient alors entrer dans ncurses depuis un autre thread, alors que les fenêtres et
les tampons sont partagés. La correction recommandée est un propriétaire unique de la console :

1. les producteurs déposent des octets ou un état GPIO dans des tampons fixes ;
2. une routine de service console traite un budget borné entre deux points d'ordonnancement ;
3. aucun autre contexte n'appelle ncurses ;
4. seules les mutations courtes des indices de files sont protégées.

Cette évolution doit rester dans le HAL hôte. Une file dynamique, un mutex POSIX exposé aux couches
supérieures ou une modification du contrat USART seraient contraires au périmètre architectural.

### P2 - la saisie rafraîchit le terminal pour chaque touche

Pour une rafale de `n` caractères, le chemin actuel effectue `n` effacements, réécrit au total
`1 + 2 + ... + n` caractères, et demande `n` rafraîchissements. La reconstruction est donc
quadratique sur une ligne, même si la longueur maximale de 63 octets la borne.

Une correction locale et peu risquée consiste à traiter toutes les touches disponibles, marquer
la ligne comme modifiée, puis appeler une seule fois `consoleInputRender()`. La validation doit
rester immédiate et la routine doit cesser de drainer dès que le tampon prêt est occupé. Remplacer
`wrefresh()` par `wnoutrefresh()` et regrouper les fenêtres avec un seul `doupdate()` permet ensuite
de fusionner les écritures terminal.

### P3 - le contrat octet par octet multiplie les masquages de signaux en réception

SCLI appelle `sc_usartRead()` jusqu'à l'erreur « tampon vide ». Chaque octet déjà copié dans
`input_ready` provoque pourtant `hal_atomicStart()` et `hal_atomicEnd()`. Une ligne de 63 octets
entraîne donc au moins 64 paires de changements de masque, dont 63 ne touchent plus ncurses.

Le contrat public USART restant volontairement unitaire, le premier palier recommandé est de sortir
le test et la copie du tampon prêt dans un chemin court, sans nouvelle interrogation ncurses. Une
lecture HAL par bloc ne doit être envisagée que dans une demande architecturale séparée, avec les
mêmes tests sur AVR et FreeBSD.

### P4 - les rafraîchissements GPIO sont immédiats et parfois inutiles

`pc_consoleLedWrite()` reformate la ligne et appelle `wrefresh()` à chaque écriture. Le cache
`signal_values` existe déjà, mais l'appelant l'écrase avant de permettre une comparaison. De plus,
une mise à jour de LED déclenche la lecture clavier, ce qui couple deux chemins sans rapport.

Il faut ignorer les écritures dont la valeur ne change pas, marquer les lignes modifiées et les
rendre avec le prochain lot console. L'initialisation doit toujours dessiner toutes les lignes.
L'interrogation clavier doit avoir un point d'appel explicite et ne plus dépendre de l'activité
GPIO.

### P5 - l'émission est déjà regroupée, mais sa politique reste implicite

Le tampon USART de 256 octets et le rafraîchissement sur fin de ligne évitent normalement un
`wrefresh()` par caractère. Ce chemin est donc meilleur que la saisie et le GPIO. Deux limites
subsistent : la boucle appelle `waddch()` pour chaque octet et une ligne de plus de 256 octets
échoue avant que tmLibc rencontre son saut de ligne.

Le premier objectif doit être de déplacer le rendu hors section critique, pas de remplacer la
boucle. Ensuite seulement, `waddnstr()` peut être comparé à `waddch()` sur des blocs sans octet nul.
La version octet par octet reste plus fidèle au contrat binaire et constitue la référence.

## Plan d'amélioration

1. Ajouter une instrumentation compilée seulement pour le banc : touches drainées, rendus de ligne,
   `wrefresh()`/`doupdate()`, octets RX/TX et durée maximale des sections critiques.
2. Regrouper le rendu d'une rafale de saisie et vérifier le contenu, le curseur, retour arrière,
   Entrée, F10, ligne vide et saturation à 63 octets.
3. Supprimer les rendus GPIO sans changement, puis regrouper entrée, sortie et GPIO avec
   `wnoutrefresh()` et un `doupdate()` par lot.
4. Introduire un propriétaire ncurses unique et limiter les sections critiques aux files fixes.
   Conserver explicitement l'ordre des octets et la politique actuelle de rejet en saturation.
5. N'optimiser la boucle TX ou le contrat RX qu'après les mesures des quatre premières étapes.

## Banc de mesure requis

Le banc doit s'exécuter sur FreeBSD dans un pseudo-terminal de taille fixe, avec le même `TERM` et
la même locale. Chaque scénario doit être répété au moins 30 fois après échauffement :

| Scénario | Charge | Mesures principales |
|---|---|---|
| RX interactif | lignes de 1, 32 et 63 octets | latence touche-affichage, appels de rendu |
| RX en rafale | collage de 1 000 lignes | débit, CPU, lignes perdues |
| TX | blocs de 1, 64, 256 octets | débit, rafraîchissements, ordre des octets |
| GPIO | valeur stable puis bascule rapide | appels évités, latence d'affichage |
| Mixte | RX, TX, GPIO et tick à 1 ms | retard maximal du tick, charge CPU |

Les seuils proposés sont : un rendu d'entrée par rafale, aucun rendu GPIO pour une valeur stable,
aucun octet perdu ou réordonné, et aucun accroissement du retard maximal du tick. Le temps CPU et le
nombre de rafraîchissements doivent diminuer ; un objectif chiffré en
microsecondes ne doit être fixé qu'après la mesure de référence.

## Tests à ajouter avant refactoring

- une fausse couche ncurses ou des wrappers injectables comptant les appels, sans terminal réel ;
- des tests de table pour caractères imprimables, trois formes d'effacement, Entrée et F10 ;
- les bornes 0, 62, 63 et 64 caractères et la conservation d'une ligne prête non consommée ;
- l'ordre exact des octets RX/TX et le traitement de `\r` en sortie ;
- l'absence de rendu GPIO sur répétition et le rendu initial complet ;
- une exécution d'intégration sous pseudo-terminal avec redimensionnement et terminal lent.

## Verdict

Le principal problème n'est pas le coût d'un appel `waddch()`, mais l'exécution de ncurses et des
E/S terminal dans des sections qui masquent le signal d'ordonnancement. Le deuxième gain le plus sûr
est le regroupement des rafales d'entrée, suivi de la suppression et du regroupement des rendus
GPIO.

La trajectoire recommandée est donc : **instrumenter, regrouper les rendus, éliminer les rendus
inchangés, puis isoler ncurses derrière un propriétaire unique**. Elle conserve les tampons fixes,
les frontières `services -> sysCall -> HAL` et le contrat USART commun, tout en réduisant les appels
terminal et la gigue du simulateur. Les changements doivent être livrés séparément de cet audit.
