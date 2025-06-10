//
// Created by dell on 6/7/2025.
//

#ifndef CLAUSES_AVL_H
#define CLAUSES_AVL_H
#include <stdlib.h>
#include <resolution.h> // Pour TVal et comparaison des clasue
#define max(a,b) ((a) > (b) ? (a) : (b))
#define min(a,b) ((a) < (b) ? (a) : (b))

#define MAXP 100	// Profondeur max

// Le modèle 'Noeud' à l'aide des macros ...

// Allouer un nouveau noeud
#define CreerNoeud()  malloc( sizeof(TNoeud) )

// Destruction du noeud p
#define LibererNoeud(p) free((p))

// Affecte v dans le noeud p
#define Aff_info(p,v) (p)->info = (v)

// Incrémente la balance de p
#define Inc_bal(p) (p)->bal++

// Décrémente la balance de p
#define Dec_bal(p)  (p)->bal--

// Affecte une valeur à la balance de p
#define Aff_bal(p,b)  (p)->bal = (b)

// Affecte q dans le fils-gauche de p
#define Aff_fg(p,q)  (p)->fg = (q)

// Affecte q dans le fils-droit de p
#define Aff_fd(p,q)  (p)->fd = (q)

// Retourne l'information de p
#define Info(p)  (p)->info

// Retourne la balance de p
#define Bal(p)  (p)->bal

// Retourne le fils-gauche de p
#define fg(p)  (p)->fg

// Retourne le fils-droit de p
#define fd(p)  (p)->fd




// Le type des valeurs
typedef clause* TVal;

// Structure d'un Noeud de l'arbre
typedef struct noeud {
    TVal info;
    int bal;
    struct noeud *fg;
    struct noeud *fd;
} TNoeud;

extern TNoeud *Pile[];	// Pile utilisée dans la recherche, l'insertion itérative et la suppression
extern int sommet;

// Les fonctions implémentées dans ce programme

/* Recherche de val dans r, retourne dans p le noeud contenant val ou NULL sinon.
   La pile globale de parcours 'Pile[]' est mise à jour aussi */
void rechercher( TVal val, TNoeud *r, TNoeud **p );

/* Insère dans l'arbre AVL de racine r, la valeur val (si elle n'existe pas déjà).
   utilise la pile globale Pile[] pour remonter dans l'arbre et mettre à jour l'équilibrage */
bool inserer( TVal val, TNoeud **r );

/* Retourne la hauteur de l'arbre AVL de racine r */
int hauteurAVL( TNoeud *r );

/* Effectue une rotation gauche autour de r, les balances sont aussi mises-à-jour */
void rotationG( TNoeud **r );

/* Effectue une rotation droite autour de r, les balances sont aussi mises-à-jour */
void rotationD( TNoeud **r );





#endif //CLAUSES_AVL_H
