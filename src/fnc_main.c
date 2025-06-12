
#include <formule_to_cnf.h>

int main(void)
{
    FILE *input_file = fopen("formula.txt", "r");
    if (!input_file) {
        fprintf(stderr, "Error opening file\n");
        return EXIT_FAILURE;
    }
    char formula[MAX_LINE_LENGTH];
    propositions_hash_table hash_table;
    init_hash_table(&hash_table);
    clause_list list_des_clauses = {NULL, NULL, 0}; // initialize the clause list
    if (fgets(formula, sizeof(formula), input_file) == NULL) {
        fprintf(stderr, "Error reading formula from file\n");
        fclose(input_file);
        return EXIT_FAILURE;
    }
    fclose(input_file); // close the input file after reading
    char *results[6];
    for (int i = 0; i < 6; i++) {
        results[i] = malloc(MAX_LINE_LENGTH * sizeof(char)); // allocate memory for each result
    }

    node root=formula_to_fnc(formula, &hash_table, &list_des_clauses,results);
    printf("Formula: %s\n", formula);
    printf("Result CNFs:\n");
    clause_node *current_node = list_des_clauses.head;
    while (current_node)
    {
        if (current_node->next!=NULL)printf("(%s) & ", clause_to_string(&current_node->value, &hash_table));
        else printf("(%s)\n", clause_to_string(&current_node->value, &hash_table));
        current_node = current_node->next; // move to the next clause node
    }

    FILE *output_file = fopen("clauses_result.txt", "w");
    if (!output_file) {
        fprintf(stderr, "Error opening output file\n");
        return EXIT_FAILURE;
    }
    save_clauses_to_file(list_des_clauses, output_file, &hash_table);
    printf("The formula has been converted to CNF and clauses saved in \"clauses_result.txt\"\n");
    fclose(output_file); // close the output file after writing
    output_file = fopen("latex_result.tex", "w");
    fprintf(output_file, "\\begin{array}{l}\n");
    if (!output_file) {
        fprintf(stderr, "Error opening LaTeX output file\n");
        return EXIT_FAILURE;
    }
    for (int i=0;i<5;i++)
    {
        switch (i)
        {
        case 0:
            fprintf(output_file, "\\textbf{Formula:} %s\\\\", results[i]); // write the original formula
            break;
        case 1:
            fprintf(output_file, " \\textbf{Remove equivalences:} %s\\\\", results[i]);
            break;
        case 2:
            fprintf(output_file, " \\textbf{Remove implications:} %s\\\\", results[i]);
            break;
        case 3:
            fprintf(output_file, " \\textbf{Push negations:} %s\\\\", results[i]);
            break;
        case 4:
            fprintf(output_file, " \\textbf{Distribute disjunctions:} %s\\\\\n", results[i]);
            break;
        }
    }
    fprintf(output_file, "\\end{array}");
    fclose(output_file); // close the LaTeX output file after writing
    printf("The formula has been converted to LaTeX format and saved in \"latex_result.txt\"\n");
    /*
     * generate truth table
     */
    printf("generating formula truth table...\n");
    char *truth_table=NULL ;
    formula_tree_truth_table(root, &hash_table, &truth_table); // generate the truth table for the formula
    output_file = fopen("truth_table.csv", "w");
    for (int i=1;i<hash_table.size;i++)
    {
        fprintf(output_file, "%s,", hash_table.table[i]);
    }
    fprintf(output_file, "%s\n",formula);
    int rows = 1 << (hash_table.size-1); // number of rows in the truth table
    for (int i = 0; i < rows; i++) {
        for (int j = 1; j < hash_table.size; j++) {
            fprintf(output_file, "%c,", (i & (1 << j)) ? 'T' : 'F'); // write the truth value for each variable
        }
        fprintf(output_file, "%c\n", truth_table[i]); // evaluate the formula for the current row
    }
    fclose(output_file);
    /*     * save the truth table to latex file
     */
    output_file = fopen("truth_tabel.tex", "w");
    fprintf(output_file, "\\begin{array}{|");
    for (int i = 1; i < hash_table.size; i++) {
        fprintf(output_file, "c%d|", i); // write the header for each variable
    }
    fprintf(output_file, "c%d|}\n", hash_table.size); // write the header for the formula
    fprintf(output_file, "\\hline\n");
    for (int i = 1; i < hash_table.size; i++) {
        fprintf(output_file, "%s & ", hash_table.table[i]); // write the variable names
    }
    fprintf(output_file, "%s \\\\\n", results[0]); // write the formula name
    fprintf(output_file, "\\hline\n");
    for (int i = 0; i < rows; i++) {
        for (int j = 1; j < hash_table.size; j++) {
            fprintf(output_file, "%c & ", (i & (1 << j)) ? 'T' : 'F'); // write the truth value for each variable
        }
        fprintf(output_file, "%c \\\\\n", truth_table[i]); // write the truth value for the formula
    }
    fprintf(output_file, "\\hline\n");
    fprintf(output_file, "\\end{array}\n");
    fclose(output_file); // close the LaTeX output file after writing
    printf("Press Enter to exit\n");
    getchar();
    return 0;
}
