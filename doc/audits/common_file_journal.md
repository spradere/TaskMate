# Audit — journal commun des écritures d'autoCode et du build

Date : 25 septembre 2026

Branche auditée : `work`

Révision auditée : `c45ecfce76c3aba33509913b27386b88d79ca04d`

Environnement d'audit : Linux 6.18.44, sans BSD `bmake`

## Objet et définition du besoin

Cet audit évalue la possibilité d'un journal commun à autoCode et aux Makefiles, contrôlé à la fin du
build, pour détecter qu'un fichier produit n'a pas été intégralement écrit ou correctement fermé. Il
couvre les sorties créées par le build normal `test1`, et non chaque fichier lu par `bmake`, le
compilateur ou les utilitaires externes.

Trois propriétés doivent être distinguées :

- **fermé** : le producteur a demandé une fermeture et en a vérifié le résultat ;
- **complet** : le contenu satisfait un contrat connu du producteur ;
- **publié** : seul un contenu complet est visible sous le nom final.

Un journal peut prouver qu'un événement a été déclaré. Il ne peut pas, à lui seul, prouver que les
octets attendus existent, que les tampons ont été durablement écrits, ni qu'un processus tué a eu le
temps de journaliser son échec. Le besoin est donc techniquement réalisable pour les **sorties gérées
et déclarées**, mais pas pour « tous les fichiers ouverts » au sens littéral.

## Verdict

La solution recommandée n'est pas un registre universel des ouvertures. C'est un **manifeste de
publication par exécution**, partagé par protocole entre autoCode et le build, associé à des écritures
temporaires puis à un renommage. Le contrôle final vérifie que chaque sortie déclarée possède un
événement terminal `committed`, que le fichier final satisfait son contrat et qu'aucun temporaire de
l'exécution ne subsiste.

La faisabilité est élevée et le coût reste moyen si le périmètre initial se limite à autoCode et aux
fichiers texte écrits directement par les recettes. La complexité devient élevée pour inclure les
objets, dépendances, exécutables et fichiers internes de tous les outils. Cette extension apporterait
peu : la fin réussie de chaque processus garantit déjà la fermeture de ses descripteurs par le noyau,
mais pas la validité sémantique de sa sortie.

Le gain de fiabilité d'un journal seul est faible. Il devient important lorsqu'il accompagne :

1. une liste exhaustive des sorties attendues ;
2. une fermeture contrôlée avant publication ;
3. une publication atomique par renommage adjacent ;
4. des critères de complétude propres à chaque type de sortie ;
5. un échec du build si le validateur final trouve un état absent ou incohérent.

| Périmètre | Faisabilité | Complexité | Gain de fiabilité | Avis |
| --- | --- | --- | --- | --- |
| Sorties autoCode | Élevée | Faible à moyenne | Élevé avec validation | Prioritaire |
| Textes produits par les Makefiles | Élevée | Moyenne | Élevé avec écriture atomique | Prioritaire |
| Objets, `.d`, ELF et rapports d'outils | Moyenne | Élevée | Faible à moyen | Hors lot initial |
| Toutes les lectures et écritures des descendants | Faible | Très élevée | Faible | À rejeter |

## État actuel

### autoCode possède déjà la majeure partie du contrôle local

Toutes ses ouvertures passent par `fileOpen()` ou `fileMakeTmp()`. `file_t` mémorise l'état ouvert et
le droit d'écriture (`srcs/autoCode/fileUtility.h:31-38`). `fileClose()` teste `ferror()`, vérifie
`fclose()` puis réinitialise l'objet (`srcs/autoCode/fileUtility.c:208-228`). Les destinations générées
sont écrites dans des temporaires, fermées, puis comparées et renommées
(`srcs/autoCode/fileUtility.c:65-101,289-302`).

Cette base rend un contrôle interne faisable sans interposition système. Il subsiste toutefois trois
limites pour le besoin étudié :

- plusieurs appels ignorent le résultat de `fileClose()`, notamment les fichiers de liste du `main`
  (`srcs/autoCode/autoCode.c:89-160`) ;
- le booléen `stream_opened` est porté par chaque objet local, sans registre de tous les flux vivants ;
- le renommage prouve la publication d'un temporaire, pas la complétude sémantique du fragment.

L'actuel résumé compte seulement les destinations modifiées ou inchangées
(`srcs/autoCode/fileUtility.c:57-63`). Il ne constitue donc pas un bilan des ouvertures et fermetures.

### Les recettes ne partagent ni processus ni abstraction de fichier

Les recettes construisent plusieurs sorties avec une troncature `>` suivie d'ajouts `>>` :

- configuration et listes autoCode (`mk/autoCode.mk:66-79,97-114`) ;
- `tm_info.h.tmp`, puis comparaison et renommage (`mk/build.mk:18-40`) ;
- manifeste de build écrit directement sous son nom final (`mk/build.mk:51-70`) ;
- agrégation des dépendances (`mk/build.mk:95-99`) ;
- `.gitignore`, rapports d'architecture, statistiques et journaux.

Chaque ligne de recette peut lancer un nouveau shell. `bmake` n'a donc pas accès aux descripteurs
internes des commandes, et un état gardé dans une variable shell ne survit pas nécessairement à la
recette suivante. Les redirections sont en outre ouvertes par le shell avant l'utilitaire appelé.

Le build est déclaré `.NOTPARALLEL` (`mk/options.mk:16-17`), ce qui simplifie un journal séquentiel,
mais les sous-makes et deux builds lancés séparément restent des producteurs distincts. Le journal
doit avoir un identifiant d'exécution et ne doit jamais être un fichier global réutilisé sans garde.

### La cible finale actuelle ne convient pas au rôle de preuve

`.END` produit `last_build_info.txt` et le résumé (`mk/build.mk:51-86`), tandis que `all` annonce la
fin après ses dépendances (`mk/build.mk:88-93`). Un validateur de fiabilité doit être une dépendance
explicite placée après les producteurs, pas seulement une commande cosmétique de `.END`. Son statut
doit participer directement au succès de `all`, et il ne doit pas valider un journal ancien lorsque le
build s'arrête avant de l'atteindre.

## Limites d'une observation universelle

### Fermeture

À la terminaison d'un processus, le noyau ferme ses descripteurs encore ouverts. Un instantané pris
après le build ne peut donc pas distinguer une fermeture explicite réussie d'une fermeture automatique.
Il ne permet pas davantage d'attribuer sûrement un descripteur hérité, dupliqué ou rouvert.

Une interception par `LD_PRELOAD` manquerait les binaires statiques, les appels système directs et les
programmes qui neutralisent l'environnement. Une trace système (`ktrace`/`truss` sur FreeBSD) serait
volumineuse, spécifique à l'hôte et complexe à corréler avec `fork`, `exec`, `dup`, `rename` et les
sorties ouvertes par le shell. Ces techniques conviennent à un diagnostic ponctuel, pas à un contrat
de build déterministe.

### Complétude

Ni `close(2)` ni un événement `closed` ne définit ce que « complet » signifie. Un fichier vide peut
être valide ; un fichier non vide peut être tronqué mais syntaxiquement plausible. Chaque classe exige
donc un contrat : nombre et noms des fragments autoCode, grammaire des listes, en-tête et dernière
ligne attendus, parseur de fichier `.d`, ou succès de l'outil producteur.

Un hash consigné après fermeture détecte une modification ultérieure, mais ne prouve pas que le bon
contenu a été produit. `fsync()` ajouterait une garantie de durabilité après perte d'alimentation, au
prix d'un coût hôte notable ; ce n'est pas nécessaire pour détecter une erreur de fermeture dans le
build local et ne doit pas être confondu avec la publication atomique.

## Architecture recommandée

### 1. Un protocole commun, pas une bibliothèque commune

autoCode en C et les recettes shell ne doivent pas partager artificiellement une bibliothèque. Ils
peuvent produire le même format de manifeste, dans un répertoire propre à l'exécution. Chaque entrée
terminale devrait au minimum contenir :

- version du protocole et identifiant unique du build ;
- producteur et chemin de destination canonique ;
- classe de sortie et critère de complétude ;
- résultat de fermeture, taille finale et, si utile, empreinte ;
- état terminal `committed`, `unchanged` ou `failed`.

Un fichier d'événements partagé et ouvert en ajout créerait lui-même le problème à contrôler : écritures
concurrentes, ligne partielle et verrouillage. Il est préférable que chaque producteur publie un petit
enregistrement atomique dans `${PATH_BUILD_TARGET}/journal/<run-id>/`, puis que le validateur agrège
ces enregistrements. Le nom final de l'enregistrement vaut engagement ; un temporaire seul vaut échec.

### 2. Un inventaire attendu déclaré par le graphe

Le validateur doit comparer les engagements à une liste préparée avant les écritures. Pour autoCode,
elle vient des fragments requis et des destinations rencontrées lors de l'analyse des tags. Pour Make,
elle doit lister explicitement les fichiers texte que le projet produit lui-même. Une entrée observée
sans attente et une attente sans entrée sont toutes deux des erreurs.

Le périmètre initial recommandé comprend : configuration et listes autoCode, fragments générés,
`tm_info.h`, `.deps.d`, rapports d'architecture et d'en-têtes, données de taille/LOC et manifeste final.
Le journal stdout d'autoCode peut rester un diagnostic : sa complétude dépend d'abord du statut de la
redirection et du processus. Les `.o`, `.d` individuels, l'exécutable autoCode et le firmware doivent
rester sous le contrat naturel « recette réussie + cible présente », sauf défaut concret ultérieur.

### 3. Publication atomique avant journalisation

Les sorties Make gérées doivent passer par un helper unique : créer un temporaire adjacent et unique,
écrire en une seule invocation, contrôler son statut, vérifier le contrat, puis le renommer. Le helper
publie ensuite son engagement. Un échec avant le renommage conserve l'ancienne destination et laisse
au validateur un état absent ou `failed`.

autoCode doit conserver son abstraction C, mais enregistrer centralement chaque `file_t` ouvert, rendre
tous les échecs de fermeture fatals avant `rename()` et publier ses engagements seulement après la
comparaison/remplacement. Un contrôle `atexit()` peut signaler les flux encore enregistrés, mais il ne
remplace pas les chemins d'erreur explicites et ne s'exécute pas après `SIGKILL` ou arrêt de la machine.

### 4. Validation ordonnée et nettoyage

Une cible interne `_file_journal_check` doit dépendre de tous les producteurs concernés et précéder le
message `Build complete`. Elle vérifie l'identifiant, l'exhaustivité, l'unicité, les états terminaux, les
critères par classe, les chemins confinés et l'absence de temporaires associés au run.

Le journal n'est marqué `complete` qu'après cette validation. Au démarrage suivant, un run non complet
est archivé comme diagnostic ou supprimé de façon confinée ; il n'est jamais accepté comme preuve du
nouveau build. Un build sans travail doit soit créer et valider un nouveau run, soit vérifier un
manifeste final versionné dont toutes les sorties sont encore cohérentes.

## Constats et priorités

### P1 — le terme « tous les fichiers ouverts » n'est pas un contrat vérifiable

Sans périmètre et sans définition de complétude, l'implémentation produirait beaucoup de traces sans
preuve utile. Elle pourrait même donner une assurance trompeuse : un couple `open/close` équilibré ne
dit rien du contenu. La première décision doit limiter le contrat aux sorties écrites et possédées par
TaskMate pendant `all`.

### P1 — le journal ne corrige pas les publications directes

Plusieurs recettes écrivent leur destination en place. Un journal terminal peut détecter certains
échecs, mais il ne restaure pas le contenu précédent. L'écriture atomique est un prérequis au gain de
fiabilité annoncé, et non une amélioration indépendante à reporter.

### P1 — une validation finale seule ne couvre pas les interruptions

Si le build est interrompu, la cible finale ne s'exécute pas. Le prochain lancement doit reconnaître
le run incomplet avant de réutiliser ses stamps ou sorties. L'identifiant du run et la publication d'un
marqueur final sont donc obligatoires.

### P2 — autoCode peut garantir ses propres flux à faible coût

Le passage centralisé par `fileUtility` permet d'ajouter un registre borné ou dynamique sur l'hôte,
des identifiants d'ouverture et un bilan final. Le principal travail est la propagation stricte des
retours de fermeture et les tests par injection de fautes, pas la journalisation elle-même.

### P2 — l'inclusion des outils externes augmenterait fortement le coût

Clang, AVR GCC, `awk`, `cloc`, Git et le shell ont leurs propres fichiers et stratégies. Les envelopper
au niveau système rendrait le build dépendant de détails FreeBSD et du comportement de versions
d'outils. Leur statut de sortie, les cibles déclarées et des validateurs de format ciblés offrent un
meilleur rapport coût/fiabilité.

## Plan d'implémentation estimé

### Étape 1 — contrat et prototype (complexité faible, 1 à 2 jours)

1. Définir le périmètre, le format versionné, les états et les critères de chaque classe.
2. Générer un identifiant par invocation de `all` et un inventaire attendu dans `build/`.
3. Prototyper le validateur sur des manifestes synthétiques, sans modifier les producteurs.

### Étape 2 — autoCode (complexité moyenne, 2 à 4 jours)

1. Enregistrer les flux ouverts et imposer la vérification de chaque fermeture.
2. Émettre un engagement atomique après chaque publication ou conservation validée.
3. Injecter les fautes d'ouverture, d'écriture, de fermeture, de renommage et d'interruption.

Cette étape est système-critique car elle touche le générateur et exige les tests normaux et
sanitizers existants, puis une revue des sorties générées.

### Étape 3 — producteurs Make (complexité moyenne à élevée, 3 à 6 jours)

1. Créer et tester le helper d'écriture atomique et d'engagement.
2. Migrer les sorties texte par petits groupes, sans reformater les Makefiles.
3. Ajouter `_file_journal_check` au graphe de `all` avec un ordre explicite.
4. Ne migrer les sorties d'outils externes que si un mode de défaillance concret le justifie.

### Étape 4 — robustesse (complexité moyenne, 2 à 4 jours)

Tester le disque plein, l'échec de fermeture, le signal, le temporaire résiduel, l'entrée dupliquée, le
run ancien, la destination modifiée après engagement et deux builds concurrents. Vérifier également un
build incrémental sans régénération et un échec survenant avant le validateur final.

L'estimation totale est de 8 à 16 jours selon le nombre de sorties Make incluses. Une interception de
tous les appels système dépasserait nettement ce coût et imposerait une maintenance continue, sans
apporter de preuve de complétude.

## Critères d'acceptation proposés

- aucun succès de `all` sans inventaire et marqueur final du même identifiant d'exécution ;
- exactement un engagement terminal par sortie attendue ;
- aucune publication si l'écriture, la validation ou la fermeture échoue ;
- aucune destination antérieure altérée après une faute avant renommage ;
- aucun temporaire du run après succès, et détection certaine après interruption ;
- aucun journal ancien accepté par un nouveau build ;
- tests compatibles avec la plateforme FreeBSD/`bmake` officielle ;
- aucune exigence ajoutée au firmware embarqué : le mécanisme reste entièrement côté hôte.

## Validation effectuée pendant l'audit

- lecture des notes d'architecture `build` et `autoCode`, des règles et de l'audit de sûreté existant ;
- inventaire statique des appels `fopen`/`fclose`, des wrappers autoCode et de leurs appelants ;
- inventaire statique des redirections, créations, déplacements et suppressions dans `mk/` et
  `scripts/` ;
- inspection du graphe `all`, de `.BEGIN`, de `.END`, de `.NOTPARALLEL` et des sorties déclarées ;
- vérification de la branche et de la révision auditées.

BSD `bmake` n'étant pas installé dans l'environnement Linux de l'audit, aucun développement du graphe
ni build `test1` n'a été exécuté. Aucun fichier généré et aucune valeur de build n'ont été modifiés.
Aucune validation sur matériel physique n'est requise pour ce rapport de conception.

## Conclusion

Un journal commun est techniquement pertinent s'il devient un protocole de **publication vérifiable**,
et non un traceur de descripteurs. Le meilleur compromis est de couvrir d'abord autoCode et les sorties
texte possédées par les Makefiles, avec inventaire préalable, temporaires adjacents, fermeture vérifiée,
renommage atomique et engagement par producteur.

Cette approche apporte un gain élevé contre les fichiers partiels, les échecs de fermeture ignorés et
les runs interrompus. Elle reste indépendante du firmware et cohérente avec le choix FreeBSD/`bmake`.
Étendre la journalisation à tous les fichiers de tous les outils serait coûteux, fragile et incapable de
prouver la complétude ; cette voie n'est pas recommandée.
