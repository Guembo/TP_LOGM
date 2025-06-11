//
// Created by dell on 6/11/2025.
//

#ifndef FORMULE_TO_CNF_H
#define FORMULE_TO_CNF_H
#include <stddef.h>
/*
* Rules of precedence between connectives:
* enables a unique reading and, thus, prevents ambiguities. In a given formula, we will apply the connectives in the following order:
* - , &, |, >, = . We will apply them from left to right when they appear consecutively in a formula.
* * - The negation operator is applied to the formula that immediately follows it.
* - The parentheses are used to group formulas and to indicate the order of precedence of the connectives.
* Connectors are confied as follows:
* * -: the negation operator
* * &: the conjunction operator
* * |: the disjunction operator
* * >: the implication operator
* * =: the equivalence operator
* ---
* Recursive definition of a formula-tree:
* * Each variable node is a formula-tree.
* * Each negation node with a formula-tree as its unique child is a formula-tree.
* * Each conjunction, disjunction, implication, or equivalence node with two formula-trees as its children is a formula-tree.
* * Any other form of a tree is not a formula-tree.
* ---
* We use the same concept of hashing propositional variable as resolution.h
 */
#define MAX_VARIABLE_LENGTH 5 // maximum length of a variable (e.g., P99)
#define MAX_LINE_LENGTH 1024
#define MAX_LITERALS 100 // maximum number of literals in a clause
#define MAX_VARIABLES 500 // maximum number of variables in the hash table
typedef struct
{
 int size; // size of hash table
 char **table; // hash table to store propositions
}propositions_hash_table;

typedef enum {
    AND, // conjunction
    OR, // disjunction
    IMPLIES, // implication
    EQUIVALENT, // equivalence
    NOT, // negation
    OPEN_BRACKET,
    CLOSE_BRACKET, // parentheses
    VARIABLE, // propositional variable
} connective_type; // type of connective
//We included variable just for programming purpose to avoid declaring another enum to distinguish connectives from variable

// We use a union to store the value of the tree node, which can be either a variable or a connector
// where variabel are stored as integers according to hashed value and
// connectors are stored as pointers to left and right children (formulas)
// each formula can be rpresented as a binary tree where each node propositional variable or a connective with one child formulas
typedef struct node_struct{
    union element_union {int variable; struct {struct node_struct* lc;struct node_struct* rc;} connector ;} val;
    char type;
}*node;

/* Functions to manupulate nodes stack */
typedef struct stack_node{
    node val;
    struct stack_node* next;
}*stack;
void creatStsack(stack* s);
int isEmpty(stack s);
void push(stack *s , node n);
node pop(stack *s);
node getTop(stack s);

#endif //FORMULE_TO_CNF_H
