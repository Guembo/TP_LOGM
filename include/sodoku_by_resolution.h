
#ifndef SODOKU_BY_RESOLUTION_H
#define SODOKU_BY_RESOLUTION_H
#include <resolution.h>
#include <clauses_avl.h>

#define SODOKU_SIZE 9 // size of the sodoku grid
#define SUB_BLOCK_SIZE 3 // size of the sub-blocks in the sodoku grid
#define ROW_INDICE 1
#define COLUMN_INDICE 10
#define CELL_INDICE 100 // indices for row, column and cell in the sodoku grid
#define USED_SODOKU_CONSTRAINTS -3 // used to mark sodoku constraints in the clause list
#define UNSUED_SODOKU_CONSTRAINTS -2
/* * Function to generate clauses for sodoku constraints
 * It returns a clause_list containing all the clauses for sodoku constraints 11988 clauses
 * includes constraints for:
 * - each cell must contain numbers from 1 to 9
 * - each row must contain numbers from 1 to 9
 * - each column must contain numbers from 1 to 9
 * - each 3x3 block must contain numbers from 1 to 9
 * * The clauses are coded as follows:
 * each cell value is represented by propositional variable of 3 digits: rcd where:
 * - r is the row index (1-9)
 * - c is the column index (1-9)
 * - d is the digit (1-9)
 */
clause_list sodoku_constraints_clauses();
/*
 *  Function to load sodoku constraints from a file
 *  The file should contain the sodoku game as follow :
 *  - each line contains 9 digits (1-9) or 0,#, or * for empty cells
 *  - each digit represents a cell in the sodoku grid
 *  - any other character found means the file is not a valid sodoku file
 *  example:
 *  123456789
 *  454000000
 *  78***47*0
 *  500000000
 *  755200000
 *  456600000
 *  78664****
 *  465100000
 *  5254552**
 */
void load_sodoku_constraints_from_file(FILE *file, clause_list *list,int startC1);

// Function to print the sodoku grid from the clause list
void print_sodoku_grid(clause_list *list);
void save_sodoku_validation(FILE *file, clause_list *list ,clause_list *result_clauses, int *used_sodoku_constraints,int size);
void sodoku_clauses_to_string(clause c, char *buffer, size_t buffer_size);
bool search_used_constraints(int c1 , int *used_sodoku_constraints, int size);
bool sodoku_resolve_by_refutaion(clause_list *list,clause_node *start);

#endif //SODOKU_BY_RESOLUTION_H
