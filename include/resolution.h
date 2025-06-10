//
// Created by dell on 6/5/2025.
//

#ifndef RESOLUTION_H
#define RESOLUTION_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
/*
 *  resolution.h
 *  this file will contain declaration of data structures and functions
 *
 *  Codification of programm :
 *  1. variable propositionel : P0, P1, P2, ..., Pn , Q0, Q1, Q2, ..., Qn... (an alphabet + index)
 *  2. ~ (negation)
 *  3. P0 & P1 (conjunction)
 *  4. P0 | P1 (disjunction)
 *  5. P0 -> P1 (implication)
 *  6. P0 <-> P1 (equivalence)
 */
#define MAX_VARIABLE_LENGHT 5 // maximum length of a variable (e.g., P99)
#define MAX_LINE_LENGTH 1024
#define MAX_LITERALS 100 // maximum number of literals in a clause
#define MAX_VARIABLES 500

typedef struct
{
    int size; // size of hash table
    char **table; // hash table to store propositions
}propositions_hash_table;

typedef struct
{
    int *literals; // array of literals
    int size; // size of the array
} clause;
typedef struct c_node
{
    clause value; // value of the clause
    int c1,c2; // indices of the clauses that were used to create this clause
    struct c_node *next; // pointer to the next clause in the linked list
}clause_node;
// Allouer un nouveau noeud
#define CreerClauseNoeud()  malloc( sizeof(clause_node) )
// Destruction du noeud p
#define LibererClauseNoeud(p) free((p))
// Affecte c1 et c2 dans le noeud p
#define Aff_clause_node(p,c1,c2) (p)->c1 = (c1); (p)->c2 = (c2)

typedef struct
{
    clause_node *head;
    clause_node *tail; // pointer to the tail of the linked list
    int size; // size of the linked list
} clause_list;
void insert_clause_node(clause_list *list, clause_node *node);

/*
 *  Function declarations
 */
clause clause_parse(char *str , propositions_hash_table *hash_table);
int proposition_index(char *proposition, propositions_hash_table *hash_table);
void init_clause(clause *c);
bool clause_search_literal(clause *c, int literal, int *found_index);
bool clasue_insert_literal(clause *c, int literal);
void init_hash_table(propositions_hash_table *hash_table);
char *clause_to_string(clause *c, propositions_hash_table *hash_table);
int clause_compare(clause *c1, clause *c2);
bool clause_resolvent(clause *c1, clause *c2, clause * result);
bool resolve_by_refutaion(clause_list *list, propositions_hash_table *hash_table);
void save_clauses_after_resolution(FILE *file, clause_list *list, propositions_hash_table *hash_table);

#endif //RESOLUTION_H
