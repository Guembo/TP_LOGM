//
// Created by dell on 6/5/2025.
//

#ifndef RESOLUTION_H
#define RESOLUTION_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
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
#define MAX_LINE_LENGTH 1024
#define MAX_LITERALS 100 // maximum number of literals in a clause

typedef struct {
    char name; // charachter
    int index;  // index
} propositional_variable;

typedef struct
{
    propositional_variable variable;
    bool sign; // 1 for positive, 0 for negative
} propositional_literal;

typedef struct
{
    propositional_literal *literals; // array of literals
    int size; // size of the array
} clause;

/*
 *  Function declarations
 */
// Function to check if two propositional variables are equal
bool pv_is_equal(propositional_variable pv1, propositional_variable pv2);
bool pl_is_equal(propositional_literal pl1, propositional_literal pl2);
bool pl_is_negation(propositional_literal pl, propositional_literal pl2);
clause clause_parse(char *str);
clause clause_create(propositional_literal *literals, int size);
propositional_literal pl_create(propositional_variable pv, bool sign);
propositional_variable pv_create(char name, int index);
void clause_str(clause c);
#endif //RESOLUTION_H
