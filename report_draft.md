## Tableau des matieres :
1. [Introduction](#introduction)
2. [Vue d’ensemble de l’implémentation](#vue-densemble-de-limplementation)
3. [Choix techniques et optimisations](#choix-techniques-et-optimisations)
4. [Tests et validation](#tests-et-validation)
5. [Module FNC](#module-fnc)
6. [Application concrète "Validation de Sodoku"](#application-concrete)
7. [Conclusion](#conclusion)
8. [Annexes ](#annexes)

## Introduction

## Vue d’ensemble de l’implémentation
Le program est divisé en plusieurs modules principaux :
- **Module FNC** : Permet de convertir une formule en forme normale conjonctive (FNC).
- **Module de résolution** : Permet de résoudre un ensemble de clauses en utilisant la méthode de résolution.
- **Module de validation de Sodoku** : Utilise le module précédent pour valider un Sodoku.

Pour codifer les formules on a utilise la notation suivante :
- `#xxx` pour une variable propositionnelle. telle que # est une variable propositionnelle chaque `xxx` est un entier. exemple : `A1` , `P43`..
- `~#xxx` pour la négation d'une variable propositionnelle. exemple : `~A1`, `~P43`..
- `(A1 | P43)` pour une disjonction de deux variables propositionnelles. exemple : `(A1 | P43)`, `(P43 | ~A1)`..
- `(A1 & P43)` pour une conjonction de deux variables propositionnelles. exemple : `(A1 & P43)`, `(P43 & ~A1)`..
- `(A1 -> P43)` pour une implication de deux variables propositionnelles. exemple : `(A1 -> P43)`, `(P43 -> ~A1)`..
- `(A1 <-> P43)` pour une équivalence de deux variables propositionnelles. exemple : `(A1 <-> P43)`, `(P43 <-> ~A1)`..

donc une clause est une liste de disjonction de variables propositionnelles. exemple : `A1 | P43 | P43 | ~A1`.

#### Module de résolution :

Prend un fichier text contenant des clauses en FNC et lit les clauses et crier une liste de clauses initiales.
Ensuite, on prend la premiere clause et génère toutes les clauses résolues possibles à partir de cette clause est tout les clauses apres elle dans la list. et c'est la clause resolvente est une nouvelle clause qui n'est pas deja dans la liste on l'ajoute à la liste des clauses .
Si on trouve une clause vide, on a prouvé que l'ensemble des clauses est inconsistant.
Si non si on ne peut pas ajouter de nouvelles clauses, on a prouvé que l'ensemble des clauses est consistant.

## Choix techniques et optimisations :

### Optimisation de coter programmation :

1. **Hashage des variables propositionnelles** :
   * Quand on lit les clauses, on utilise un tableau de hachage pour associer à chaque variable propositionnelle un entier unique. Cela permet de réduire la taille des clauses qui sont representer par leur tail et un tableau d'entier pour les latéraux, et d'accélérer les opérations de comparaison. et dana les clauses les leterals sont stockés sous forme d'entiers positif ou négatif celon si la variable est négative ou positive.
   * D'ou l'operation de comparaison entre deux lateral est réduite à une simple comparaison d'entiers de complexité O(1) quelle que soit la taille des latéraux, Et meme pour recuperer la variable propositionnelle associée à un entier, on utilise le tableau de hachage pour retrouver rapidement la variable correspondante avec une complexité O(1) peu importe le nombre de variables propositionnelles.
   * Cette method permet de codifier jusqu'à 2^63-1 variables propositionnelles, ce qui est largement suffisant pour la plupart des applications pratiques.<br>
   >exmple : `P24` est stocké comme `1`, `~P24` est stocké comme `-1`, `Q` est stocke comme `2`, `P47` est stocké comme `3`.<br>
pour afficher les clauses ou les sauvegarder les resultats on utilise un tableau de hachage pou associer chaque entier a sa variable propositionnelle.<br>
   ![](res/images/hashing_visualisatioin.png "Image")

2. **Ordonnement des leterals dans les clauses** : <br>
   * On stocke les lateral dans des clauses dans un ordre croissant. Cela permet d'accélérer les opérations de resolution en réduisant le nombre de comparaisons nécessaires pour trouver les leterals communs entre deux clauses.<br>
   Par exemple, si on a deux clauses `(A1 | P43 | ~C)` et `(P43 | ~A1)`, on les stocke sous forme de listes ordonnées `[-3, 1, 2]` et `[-1, 2]` respectivement. Cela permet de comparer les clauses plus rapidement.<br>
   * Donc la comparaison de deux clauses est réduite à une simple comparaison de listes ordonnées, avec une complexité O(n) pour comparer, avec n le nombre de leterals dans la clause la plus longue.<br>

3. **Associe une arbe AVL a la liste des clauses** :
   * On utilise une arbe AVL pour stocker les clauses. Cela permet de chercher rapidement si une clause existe déjà dans la liste des clauses qui est tres util surtouts dans le cas d'un grand nombre de clauses, avec une complexité O(log n) pour la recherche.<br>
   * Pour cela, on a definer une relation d'ordre sur les clauses, qui est basée sur la comparaison entre les tableaux d'entiers des lateraux pour qu'on puisse utiliser l'arbre AVL.
   * Par exemple, si on a deux clauses `[-3, 1, 2]` et `[-1, 2]` la premiere clause est plus petite que la deuxieme clause car le premier element de la premiere clause est plus petit que le premier element de la deuxieme clause.<br>
   `[-3, -2, 1] < [-3, -1]`

### Optimisation de coter logique :