#include <stdio.h>
#include <stdlib.h>

#include "resolution.h"
#include <time.h>  	// pour time() et time_t
#include <sys/time.h> 	// pour gettimeofday(...) et struct timeval

#include "sodoku_by_resolution.h"
/***********************************/
/*** Programme Principal	 ***/
/***********************************/
enum
{
    SOLVE_BY_REFUTATION,
    GENERATE_CLAUSES
} program_mode=-1;


/* * Function to load clauses from a file into a clause_list
 * It reads each line, parses it into a clause, and adds it to the list
 */
void load_clauses_from_file(FILE *file, clause_list *list, propositions_hash_table *hash_table);
/* * Function to save clauses after resolution to a file
 * It writes each clause in the list to the file in a specific format
 */
void save_clauses_after_resolution(FILE *file, clause_list *list, propositions_hash_table *hash_table);

int main(int argc, char **argv)
{
    FILE *input_file = NULL; // pointer to the input file
    if (argc == 2 && strcmp(argv[1], "-h") == 0) {
        printf(
            "Usage:\n"
            "  %s -h\n"
            "------Display this help message.\n"
            " %s\n"
            "------Resolve clauses by refutation from file ./clauses.txt .\n"
            "  %s <input_file>\n"
            "------<input_file> should contain a list of clauses to resolve.\n"
            "  %s -g <input_file>\n"
            "------<input_file> should contain a formula; the program will generate a text file with its clauses.\n",
            argv[0], argv[0],argv[0],argv[0]);
        return 16;
    }
    if (argc > 3) {
        fprintf(stderr, "Too many arguments, expected 1 or 2 arguments\n");
        return EXIT_FAILURE;
    }
    if (argc == 3 && strcmp(argv[1], "-g") != 0) {
        fprintf(stderr, "Invalid option '%s', expected '-g'\n", argv[1]);
        return EXIT_FAILURE;
    }

    if (argc == 3 && strcmp(argv[1], "-g") == 0) {
        // Generate clauses from the input file
        program_mode = GENERATE_CLAUSES;
        input_file = fopen(argv[2], "r");
    } else if (argc == 2) {
        // Read clauses from the input file
        program_mode = SOLVE_BY_REFUTATION;
        input_file = fopen(argv[1], "r");
    } else {
        // Default mode: solve by refutation from the file ./clauses.txt
        program_mode = SOLVE_BY_REFUTATION;
        input_file = fopen("./clauses.txt", "r");
    }

    if (input_file == NULL) {
        fprintf(stderr, "Error: Could not open input file '%s'\n", (argc == 3 ? argv[2] : "clauses.txt"));
        return EXIT_FAILURE;
    }

    long t; 			// nombre de microsecondes
    struct timeval tv1, tv2; 	// pour gettimeofday(...)

    clause_list list_des_clauses= {NULL, NULL, 0}; // initialize the clause list
    propositions_hash_table  hash_table ;
    init_hash_table(&hash_table); // initialize the hash table

    switch (program_mode)
    {
        case GENERATE_CLAUSES:
            fprintf(stderr, "Generating clauses from the input file is not implemented yet\n");
            return EXIT_FAILURE;

        case SOLVE_BY_REFUTATION:
            int choice;
            printf("Reading clauses from the input file '%s'\n", (argc == 2 ? argv[1] : "clauses.txt"));
            gettimeofday(&tv1, NULL);
            load_clauses_from_file(input_file, &list_des_clauses, &hash_table);
            gettimeofday(&tv2, NULL);
            t = (tv2.tv_sec - tv1.tv_sec)*1000000 + (tv2.tv_usec - tv1.tv_usec);
            printf("Loading clauses took %ld ms\n",t/1000);
            printf("Number of clauses loaded: %d\n", list_des_clauses.size);
            fflush(stdout);
            fclose(input_file); // close the input file after reading
            printf("Solving clauses by refutation...\n");
        clause_node * node=list_des_clauses.head;
        for (int i=0;i<list_des_clauses.size;i++)
        {
            printf("%s\n",clause_to_string(&node->value, &hash_table));
            node = node->next;
        }
            gettimeofday(&tv1, NULL);
            bool result = resolve_by_refutaion(&list_des_clauses,NULL, &hash_table);
            gettimeofday(&tv2, NULL);
            t = (tv2.tv_sec - tv1.tv_sec)*1000000 + (tv2.tv_usec - tv1.tv_usec);
            if (result == true) {
                printf("Resolution by refutation succeeded, contradiction found.\n");
            } else {
                printf("Resolution by refutation failed, no contradiction found.\n");
            }
            printf("Operation took %ld ms\n",t/1000);
            fflush(stdout);
            FILE *deduction_file = fopen("deduction.csv", "w");
            if (deduction_file == NULL) {
                fprintf(stderr, "Error: Could not open output file 'deduction.csv'\n");
                return EXIT_FAILURE;
            }
        save_clauses_after_resolution(deduction_file, &list_des_clauses, &hash_table);
        fclose(deduction_file);

        break;
        default:
            fprintf(stderr, "Unknown program mode %d\n", program_mode);
            return EXIT_FAILURE;
    }

    return 0;
}

void load_clauses_from_file(FILE *file, clause_list *list, propositions_hash_table *hash_table)
{
    int counter = 0;
    char line[MAX_LINE_LENGTH]; // buffer to read lines from the file
    while (fgets(line, sizeof(line), file)) { // read each line from the file
        if (line[0] == '\n' || line[0] == '#') continue; // skip empty lines and comments
        clause c = clause_parse(line, hash_table); // parse the clause from the line
        if (c.size > 0) { // if the clause is not empty
            clause_node *new_node = CreerClauseNoeud(); // create a new node for the clause
            new_node->value = c; // set the value of the node to the clause
            int c1=counter++ , c2=-1; // initialize c1 and c2 to -1
            Aff_clause_node(new_node, c1, c2); // initialize c1 and c2 to -1 for initial clauses
            new_node->next = NULL; // initialize next pointer to NULL
            if (list->head == NULL) { // if the list is empty
                list->head = new_node; // set head to the new node
                list->tail = new_node; // set tail to the new node
            } else {
                list->tail->next = new_node; // append the new node to the end of the list
                list->tail = new_node; // update tail to the new node
            }
            list->size++; // increment size of the list
        }
    }
}

