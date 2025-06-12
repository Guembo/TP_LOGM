
#ifndef FORMULE_TO_CNF_H
#define FORMULE_TO_CNF_H
#include <stddef.h>
#include <resolution.h>
#include <clauses_avl.h>
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


typedef enum {
    AND='&', // conjunction
    OR='|', // disjunction
    IMPLIES='>', // implication
    EQUIVALENT='=', // equivalence
    NOT='-', // negation
    OPEN_BRACKET='(', // opening parenthesis
    CLOSE_BRACKET=')', // parentheses
    VARIABLE, // propositional variable
} connective_type; // type of connective
/*enum
{
    B_AND = 0xA7, // using UTF-8 characters for beautiful connectors
    B_OR = 0x1E, // using UTF-8 characters for beautiful connectors
    B_IMPLIES = 0x92, // using UTF-8 characters for beautiful connectors
    B_EQUIVALENT = 0x94, // using UTF-8 characters for beautiful connectors
    B_NOT = 0xAC, // using UTF-8 characters for beautiful connectors
}BEAUTIFUL_CONECTORS; // using UTF-8 characters for beautiful connectors*/
enum {FIRST_FORMULA , RIGHT_FORMULA , LEFT_FORMULA }; // used to distinguish left and right formulas in the tree
//We included variable just for programming purpose to avoid declaring another enum to distinguish connectives from variable

// We use a union to store the value of the tree node, which can be either a variable or a connector
// where variabel are stored as integers according to hashed value and
// connectors are stored as pointers to left and right children (formulas)
// each formula can be rpresented as a binary tree where each node propositional variable or a connective with one child formulas
typedef struct node_struct{
    union element_union {int variable; struct {struct node_struct* lc;struct node_struct* rc;} connector ;} val;
    char type;
}*node;
void allocateTree(node * n);
void destroy_tree(node *root);
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

/**/
int isConnector(char c);
int comparePriority(char a, char b);
int variable_compare(char *a, char *b);
node formula_to_tree(char *formula, propositions_hash_table *hash_table);
void latex_tree_to_formula(node root, propositions_hash_table hash_table, char *buffer, int *index, int formula_type,int parenthese);
void tree_to_formula(node root,propositions_hash_table hash_table ,char *buffer,int *index,int formula_type);
void tree_to_formula_reduced_parentheses(node root,propositions_hash_table hash_table ,char *buffer,int *index,int formula_type,int parenthese);
void append_utf8(char *buffer, int *index, const char *utf8_char);
node formula_tree_duplicate(node root);
void formula_tree_remove_equivalences(node *root);
void formula_tree_remove_implications(node *root);
void formula_tree_push_negation(node *root);
void formula_tree_distribute_disjunctions(node *root);
node formula_to_fnc(char *formula, propositions_hash_table *hash_table, clause_list *list,char *results[]);
void clause_to_latex(clause c, propositions_hash_table *hash_table, char *buffer, int *index);
int formula_tree_evaluate(node root, propositions_hash_table *hash_table, int *values);
void formula_tree_truth_table(node root, propositions_hash_table *hash_table,char **truth_table);
#endif //FORMULE_TO_CNF_H
