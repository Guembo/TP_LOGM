#include <resolution.h>
#include <sodoku_by_resolution.h>



int main(int argc, char *argv[])
{
    if (argc > 2) {
        fprintf(stderr, "Usage: %s [input_file]\nIf no file is specified, 'sodoku.txt' in the current directory will be used automatically.\n", argv[0]);
        return EXIT_FAILURE;
    }
    FILE *input_file = NULL; // pointer to the input file
    char *input_file_name = NULL;
    if (argc == 2) {
        input_file_name = argv[1]; // get the input file name from the command line argument
    } else {
        input_file_name = "sodoku.txt\0"; // default input file name
    }
    input_file = fopen(input_file_name, "r"); // open the input file for reading
    if (input_file == NULL) {
        fprintf(stderr, "Error: Could not open input file '%s'\n", input_file_name);
        return EXIT_FAILURE; // exit if the file cannot be opened
    }

    printf("Generating sodoku constraints clauses...\n");
    clause_list list = sodoku_constraints_clauses(); // generate the sodoku constraints clauses
    printf("Sodoku constraints clauses generated successfully.\n");

    clause_list intial_values = {NULL, NULL, 0}; // initialize the list of initial values
    load_sodoku_constraints_from_file(input_file, &intial_values,list.size); // load the sodoku constraints from the input file
    fclose(input_file); // close the input file after reading
    printf("Sodoku constraints game loaded successfully.\n");
    printf("Number of non-empty cells loaded: %d\n", intial_values.size); // print the number of clauses loaded
    printf("The sodoku game :\n");
    print_sodoku_grid(&intial_values);

    // append the sodoku constraints clauses to the list
    if (intial_values.size == 0)
    {
        printf("The grid is empty, no clauses to resolve.\n");
        return EXIT_SUCCESS; // if the list is empty, exit successfully
    }






    int initial_size = intial_values.size; // store the initial size of the list
    int constraint_size = list.size;
    list.size += initial_size; // update the size of the list
    list.tail->next = intial_values.head; // link the tail of the list to the head of the sodoku constraints clauses
    list.tail = intial_values.tail;

    int size = list.size; // get the size of the list
    clause_node *old_head = list.head;
    int result = sodoku_resolve_by_refutaion(&list,intial_values.head); // resolve the clauses by refutation
    //int result = resolve_by_refutaion(&list, intial_values.head, NULL); // resolve the clauses by refutation

    if (result) {
        printf("The sodoku grid is invalid\n");
    } else {
        printf("valid sodoku grid.\n");
    }

    int resolved_clauses_count = list.size - size; // count the number of resolved clauses
    // make the resolvent clauses at the end of list
    if (resolved_clauses_count >0)
    {
        clause_node *current = list.head;
        clause_node *res_head = current;
        clause_node *p = current->next;
        clause_node *tail = current;
        current->next = NULL;
        while (p->c2 != UNSUED_SODOKU_CONSTRAINTS)
        {
            res_head = p;
            p = p->next; // move to the next node in the list
            res_head->next = current;
            current = res_head;
        }
        list.head = p;
        list.tail->next = res_head;
        list.tail = tail; // update the tail of the list to the last node
    }
    int used_sodoku_constraints[resolved_clauses_count] ;
    clause_node *current_node = intial_values.tail->next; // pointer to the first created clause by resolution
    int used_count = 0; // count of used sodoku constraints
    while (current_node != NULL) {
        if (current_node->c2 <= initial_size+constraint_size-1 && current_node->c2 >= initial_size) {
            used_sodoku_constraints[used_count++] = current_node->c2; // mark the sodoku constraints as used
        }
        current_node = current_node->next; // move to the next node in the list
    }


    FILE *output_file = NULL;
    output_file = fopen("sodoku_solution.csv", "w"); // open the output file for writing
    save_sodoku_validation(output_file, &list, &intial_values,used_sodoku_constraints,used_count-1); // save the sodoku validation to the output file

}




clause_list sodoku_constraints_clauses()
{
    int c1 = 0; // count of clauses created
    int c2=UNSUED_SODOKU_CONSTRAINTS;
    clause_list list = {NULL, NULL, 0}; // initialize the clause list
    // cells contain numbers from 1 to 9
    for (int i=1;i<=SODOKU_SIZE;i++){
    for (int j=1;j<=SODOKU_SIZE ;j++){
        // create a clause for each cell that contains numbers from 1 to 9


        clause_node *new_clause_node = CreerClauseNoeud(); // create a new clause node
        new_clause_node->value.size = SODOKU_SIZE; // set the size of the clause to 9
        new_clause_node->value.literals = malloc(SODOKU_SIZE * sizeof(int)); // allocate memory for the literals
        Aff_clause_node(new_clause_node,c1,c2); // set c1 and c2 for the clause node
        c1++; // increment the count of clauses created
        new_clause_node->next = NULL; // set the next pointer to NULL
        for (int k=1;k<=SODOKU_SIZE;k++)
        {
            new_clause_node->value.literals[k-1] = i * ROW_INDICE + j * COLUMN_INDICE + k*CELL_INDICE; // set the literals to the cell values
        }
        insert_clause_node(&list, new_clause_node); // insert the clause node into the list


        for (int k=1;k<=SODOKU_SIZE;k++){
        for (int l=k+1;l<=SODOKU_SIZE;l++){
        if (k != l){
            // create a clause for each pair of literals that cannot be true at the same time
            clause_node* new_pair_node = CreerClauseNoeud(); // create a new clause node
            new_pair_node->value.size = 2; // set the size of the clause to 2
            new_pair_node->value.literals = malloc(2 * sizeof(int)); // allocate memory for the literals
            // -l < -k so to keep the literals in ascending order add j before
            new_pair_node->value.literals[0] = -(i * ROW_INDICE + j * COLUMN_INDICE + l * CELL_INDICE);
            new_pair_node->value.literals[1] = -(i * ROW_INDICE + j * COLUMN_INDICE + k * CELL_INDICE);
            new_pair_node->next = NULL; // set the next pointer to NULL
            Aff_clause_node(new_pair_node,c1,c2); // set c1 and c2 for the clause node
            c1++; // increment the count of clauses created
            insert_clause_node(&list, new_pair_node); // insert the clause node into the list
        }}}

        // constraint : for rows and columns each one must contain numbers from 1 to 9
        clause_node* row_node = CreerClauseNoeud(); // create a new clause node for the row
        row_node->value.size = SODOKU_SIZE; // set the size of the clause to 9
        row_node->value.literals = malloc(SODOKU_SIZE * sizeof(int)); // allocate memory for the literals
        Aff_clause_node(row_node,c1,c2); // set c1 and c2 for the clause node
        c1++; // increment the count of clauses created
        row_node->next = NULL; // set the next pointer to NULL
        for (int k=1;k<=SODOKU_SIZE;k++)
        {
            row_node->value.literals[k-1] = i * ROW_INDICE + k * COLUMN_INDICE + j * CELL_INDICE; // set the literals to the row values
        }
        insert_clause_node(&list, row_node); // insert the row clause node into the list)

        clause_node* col_node = CreerClauseNoeud();
        col_node->value.size = SODOKU_SIZE; // set the size of the clause to 9
        col_node->value.literals = malloc(SODOKU_SIZE * sizeof(int)); // allocate memory for the literals
        Aff_clause_node(col_node,c1,c2); // set c1 and c2 for the clause node
        c1++; // increment the count of clauses created
        col_node->next = NULL; // set the next pointer to NULL
        for (int k=1;k<=SODOKU_SIZE;k++)
        {
            col_node->value.literals[k-1] = k * ROW_INDICE + i * COLUMN_INDICE + j * CELL_INDICE; // set the literals to the column values
        }
        insert_clause_node(&list, col_node); // insert the row clause node into the list
    }}

    // constraints for at most one occurence of each digit in each row and column
    for (int i=1;i<=SODOKU_SIZE;i++) {// loop through rows
    for (int j=1;j<=SODOKU_SIZE;j++)  {// loop throught digits
    for (int k=1;k<=SODOKU_SIZE;k++) {// loop through digits 1 to 9
    for (int l=k+1;l<=SODOKU_SIZE;l++) {// loop through digits 1 to 9
    if (k != l) {// if the digits are different
        // create a clause for each pair of literals that cannot be true at the same time
        clause_node* new_pair_node = CreerClauseNoeud(); // create a new clause node
        new_pair_node->value.size = 2; // set the size of the clause to 2
        new_pair_node->value.literals = malloc(2 * sizeof(int)); // allocate memory for the literals
        // -l < -k so to keep the literals in ascending order add j before
        new_pair_node->value.literals[0] = -(i * ROW_INDICE + l * COLUMN_INDICE + j * CELL_INDICE);
        new_pair_node->value.literals[1] = -(i * ROW_INDICE + k * COLUMN_INDICE + j * CELL_INDICE);
        new_pair_node->next = NULL; // set the next pointer to NULL
        Aff_clause_node(new_pair_node,c1,c2); // set c1 and c2 for the clause node
        c1++; // increment the count of clauses created
        insert_clause_node(&list, new_pair_node); // insert the clause node into the list
    }}}}} // loop through rows end

    for (int i=1;i<=SODOKU_SIZE;i++) {// loop through columns
    for (int j=1;j<=SODOKU_SIZE;j++)  {// loop throught digits
    for (int k=1;k<=SODOKU_SIZE;k++) {// loop through rows
    for (int l=k+1;l<=SODOKU_SIZE;l++) {// loop through rows
    if (k != l) {// if the digits are different
        // create a clause for each pair of literals that cannot be true at the same time
        clause_node* new_pair_node = CreerClauseNoeud(); // create a new clause node
        new_pair_node->value.size = 2; // set the size of the clause to 2
        new_pair_node->value.literals = malloc(2 * sizeof(int)); // allocate memory for the literals
        // -l < -k so to keep the literals in ascending order add j before
        new_pair_node->value.literals[0] = -(l * ROW_INDICE + i * COLUMN_INDICE + j * CELL_INDICE);
        new_pair_node->value.literals[1] = -(k * ROW_INDICE + i * COLUMN_INDICE + j * CELL_INDICE);
        new_pair_node->next = NULL; // set the next pointer to NULL
        Aff_clause_node(new_pair_node,c1,c2); // set c1 and c2 for the clause node
        c1++; // increment the count of clauses created
        insert_clause_node(&list, new_pair_node); // insert the clause node into the list
    }}}}} // loop through columns end

    // constraints for 3x3 blocks each block must contain numbers from 1 to 9
    // let i , j be the indices of the block
    // loop through the blocks vertically
    for (int i=1;i<=SODOKU_SIZE;i+=SUB_BLOCK_SIZE){ // loop through the blocks horizontally
    for (int j= 1;j<=SODOKU_SIZE;j+=SUB_BLOCK_SIZE){ // loop through the blocks vertically
        // loop through digits 1 to 9 then block cells to make sure each digit appears at least once in the block
        for (int k=1;k<=SODOKU_SIZE;k++){
            clause_node* new_clause_node = CreerClauseNoeud();
            new_clause_node->value.size = SODOKU_SIZE; // set the size of the clause to 9
            new_clause_node->value.literals = malloc(SODOKU_SIZE * sizeof(int)); // allocate memory for the literals
            Aff_clause_node(new_clause_node,c1,c2); // set c1 and c2 for the clause node
            c1++; // increment the count of clauses created
            new_clause_node->next = NULL; // set the next pointer to NULL
            int index = 0;
            for (int l=i;l<i+SUB_BLOCK_SIZE;l++){ // loop through the rows of the block
            for (int m =j;m<j+SUB_BLOCK_SIZE;m++){ // loop through the columns of the block
                new_clause_node->value.literals[index++] = l * ROW_INDICE + m * COLUMN_INDICE + k * CELL_INDICE; // set the literals to the block values
                for (int n=l;n<i+SUB_BLOCK_SIZE;n++)
                {
                    for (int o =j;o<j+SUB_BLOCK_SIZE;o++)
                    {
                        if (n>l || (n==l && o>m)) // to avoid duplicates
                        {
                            clause_node* new_pair_node = CreerClauseNoeud(); // create a new clause node
                            new_pair_node->value.size = 2; // set the size of the clause to 2
                            new_pair_node->value.literals = malloc(2 * sizeof(int)); // allocate memory for the literals
                            // -l < -k so to keep the literals in ascending order add j before
                            new_pair_node->value.literals[0] = -(n * ROW_INDICE + o * COLUMN_INDICE + k * CELL_INDICE);
                            new_pair_node->value.literals[1] = -(l * ROW_INDICE + m * COLUMN_INDICE + k * CELL_INDICE);
                            new_pair_node->next = NULL; // set the next pointer to NULL
                            Aff_clause_node(new_pair_node,c1,c2); // set c1 and c2 for the clause node
                            c1++; // increment the count of clauses created
                            insert_clause_node(&list, new_pair_node); // insert the clause node into the list
                        }
                    }
                }
            }}
            insert_clause_node(&list, new_clause_node);
        }

    }} // end of the loop through the blocks
    return list; // return the list of clauses
}


void load_sodoku_constraints_from_file(FILE *file, clause_list *list , int startC1)
{
    if (file == NULL) {
        fprintf(stderr, "Error: Could not open file for sodoku constraints\n");
        exit(EXIT_FAILURE);
    }
    char line[SODOKU_SIZE+1] = {'x'}; // buffer to read lines from the file
    int c1=startC1, c2=-1; // initialize c1 and c2 to -1
    for (int i=0;i<SODOKU_SIZE;i++) { // loop through rows
        if (fgets(line, sizeof(line), file) == NULL) {
            fprintf(stderr, "Error: Could not read line %d from file\n", i+1);
            exit(EXIT_FAILURE);
        }
        line[SODOKU_SIZE] = '\0';
        if (line[0] == '\n' || line[0] == '#') { // skip empty lines and comments
            i--; // decrement i to repeat this row
            continue; // continue to the next iteration
        }
        for (int j=0;j<SODOKU_SIZE;j++) { // loop through columns
            if (line[j]<= '9' && line[j] >= '1') { // if the character is a digit
                clause_node *new_clause_node = CreerClauseNoeud(); // create a new clause node
                new_clause_node->value.size = 1; // set the size of the clause to 1
                new_clause_node->value.literals = malloc(sizeof(int)); // allocate memory for the literals
                new_clause_node->value.literals[0] = (i + 1) * ROW_INDICE + (j + 1) * COLUMN_INDICE + (line[j] - '0') * CELL_INDICE; // set the literal to the cell value
                Aff_clause_node(new_clause_node, c1, c2); // set c1 and c2 for the clause node
                c1++; // increment the count of clauses created
                new_clause_node->next = NULL; // set the next pointer to NULL
                insert_clause_node(list, new_clause_node); // insert the clause node into the list
                line[j] = 'x'; // reset the character to catch uncompleted lines
            } else if (line[j] != '#' && line[j] != '0' && line[j] != '*') { // if the character is not a digit or empty cell
                fprintf(stderr, "Error: Invalid character '%c' in sodoku constraints file at row %d, column %d\n", line[j], i+1, j+1);
                exit(EXIT_FAILURE);
            }
        }
    }
}

void print_sodoku_grid(clause_list *list)
{
    char grid[SODOKU_SIZE][SODOKU_SIZE];
    for (int i=0;i<SODOKU_SIZE;i++) { // initialize the grid with empty cells
        for (int j=0;j<SODOKU_SIZE;j++) {
            grid[i][j] = '*'; // use '#' to represent empty cells
        }
    }
    clause_node *node;
    node = list->head;
    while (node != NULL)
    {
        int row = (node->value.literals[0] / ROW_INDICE) % 10 - 1; // get the row index
        int col = (node->value.literals[0] / COLUMN_INDICE) % 10 - 1; // get the column index
        int digit = (node->value.literals[0] / CELL_INDICE) % 10; // get the digit
        grid[row][col] = digit+ '0'; // set the cell value in the grid
        node = node->next;
    }
    for (int i=0;i<SODOKU_SIZE;i++) { // loop through rows
        for (int j=0;j<SODOKU_SIZE;j++) { // loop through columns
            printf("%c ", grid[i][j]); // print the cell value
        }
        printf("\n"); // print a new line after each row
    }
    fflush(stdout);
}

void save_sodoku_validation(FILE *file, clause_list *list ,clause_list *result_clauses, int *used_sodoku_constraints,int size)
{
    int index = 0; // index for the clauses
    clause_node *node = list->head; // start with the head of the list
    char variable[MAX_LINE_LENGTH];
    while (node != NULL ) { // iterate through the list
        if (node->c2 == UNSUED_SODOKU_CONSTRAINTS)
        {
            if (search_used_constraints(node->c1, used_sodoku_constraints, size)) { // if the constraint is used, skip it
                node->c2 = USED_SODOKU_CONSTRAINTS; // mark the constraint as used
            }
        }
        sodoku_clauses_to_string(node->value, variable, sizeof(variable)); // convert the clause to a string
        if (node->c2 == -1) { // if c2 is -1, it means this is an initial clause
            fprintf(file, "c%d,,%s,initial value\n", index++, variable); // print the clause to the file
        } else if (node->c2 == UNSUED_SODOKU_CONSTRAINTS) { // if c2 is UNSUED_SODOKU_CONSTRAINTS, it means this is a sodoku constraint clause
            fprintf(file, "c%d,,%s,sodoku constraints\n", index++, variable); // print the resolvent clause with its origin
        }else if (node->c2 == USED_SODOKU_CONSTRAINTS) // if c2 is USED_SODOKU_CONSTRAINTS, it means this is a used sodoku constraint clause
        {
            fprintf(file, "c%d,,%s,*sodoku constraints\n", index++, variable); // print the resolvent clause with its origin

        } else {
            fprintf(file, "c%d,res(c%d;c%d),%s,resolvent\n", index++, node->c1, node->c2, variable); // print the resolvent clause with its origin
        }
        node = node->next; // move to the next node
        // if (node !=NULL && node->c2 == UNSUED_SODOKU_CONSTRAINTS)
        // {
        //     index = result_clauses->tail->c1+1;
        //     node = result_clauses->tail;
        // }
    }
}
void sodoku_clauses_to_string(clause c, char *buffer, size_t buffer_size)
{
    if (buffer == NULL || buffer_size == 0) {
        fprintf(stderr, "Error: Buffer is NULL or size is zero\n");
        return;
    }
    buffer[0] = '\0'; // initialize the buffer
    for (int i = 0; i < c.size; i++) { // loop through the literals in the clause
        int j = abs(c.literals[i]);
        int row = (j / ROW_INDICE) % 10 - 1; // get the row index
        int col = (j / COLUMN_INDICE) % 10 - 1; // get the column index
        int digit = (j / CELL_INDICE) % 10; // get the digit
        char variable[6]; // buffer to store the variable string
        int index = 0;
        if (c.literals[i]<0) {
            variable[index++] = '-';
        }
        variable[index++] = 'X'; // use 'X' to represent the variable
        variable[index++] = '0' + row + 1; // set the row index in the variable
        variable[index++] = '0' + col + 1; // set the column index in the variable
        variable[index++] = '0' + digit; // set the digit in the variable
        variable[index++] = '\0'; // null-terminate the string
        strcat(buffer, variable); // append the variable to the buffer
        if (i < c.size - 1) {
            strncat(buffer, " | ", buffer_size - strlen(buffer) - 1); // add a space between variables
        }
    }
}

bool search_used_constraints(int c1 , int *used_sodoku_constraints, int size)
{
    for (int i=0;i<size;i++)
    {
        if (used_sodoku_constraints[i] == c1) return true; // if the constraint is used, return true
    }
    return false; // if the constraint is not used, return false
}

bool sodoku_resolve_by_refutaion(clause_list *list,clause_node *start)
{
    // craate an AVL tree to store clauses to perform search for already existing clauses efficiently
    TNoeud *root = NULL; // root of the AVL tree
    clause_node *clause_list = list->head; // pointer to the clause list
    while (clause_list != NULL) // iterate through the list of clauses
    {
        // insert clause into the AVL tree
        clause *c = &clause_list->value; // get the clause from the list
        inserer(c, &root); // insert the clause into the AVL tree
        clause_list = clause_list->next; // move to the next clause in the list
    }
    // now we have all clauses in the AVL tree, we can perform resolution by refutation
    clause_node *node1 , *node2; // pointers to the clauses in the list
    clause_node *resolvent_node; // pointer to the resolvent node
    clause resolvent = {0}; // initialize the resolvent clause
    if (start != NULL) { // if start is not NULL, we start from the given clause
        node1 = start; // set node1 to start
    } else {
        node1 = list->head; // otherwise, start with the first clause in the list
    }
    int c1 = node1->c1-1;
    int c =list->tail->c1 ;
    int resolvent_count = 0; // count of resolvents created
    int counter ;
    while (node1 != NULL)
    {
        //if (node1->c2 == -2) break;
        c1++; // increment c1 for each clause
        //node2 = node1->next;

            //c2 = c1; // set c2 to c1 for initial clauses
            //node2 = node1->next;
            node2 = list->head;
            counter= -1; // reset c2 for each new node1
        while (node2 != node1)
        {
            counter++;
            /*if (node1->c1 == node2->c1 && node1->c2!=-1)
            {
                node2 = node1->next;
                c2 = c1+1;
                continue;
            }*/
            //for (int i=0;i<node2->value.size;i++) printf(" %d ",node2->value.literals[i]);
            //printf("\n%d: %s || %s \n",ss++,clause_to_string(&node1->value,hash_table),clause_to_string(&node2->value, hash_table));
            if (clause_resolvent(&node1->value,&node2->value,&resolvent))
            {
                resolvent_node = CreerClauseNoeud(); // create a new clause node for the resolvent
                resolvent_node->value = resolvent; // set the value of the resolvent
                resolvent_node->next = NULL; // set the next pointer to NULL

                if (inserer(&resolvent_node->value, &root)){ // insert the resolvent into the AVL tree , if it is not already present insert it in list

                    // in this sodoku version resolvent clause are added to the head of the list
                    list->size++; // increment the size of the list
                    resolvent_node->next = list->head;
                    list->head = resolvent_node; // set the head to the new node

                    if (node2->c2 != UNSUED_SODOKU_CONSTRAINTS) // if the node2 is not a sodoku constraint clause
                    {
                        int c2 = resolvent_count-counter +c;
                        Aff_clause_node(resolvent_node, c1, c2); // set c1 and c2 for the resolvent node
                    } else {
                        int c2 = node2->c1;
                        Aff_clause_node(resolvent_node, c1, c2); // set c1 and c2 for the resolvent node
                    }

                    resolvent_count++;
                    if (resolvent.size==0)
                    {
                        //printf("FOUNDED\n");
                        //printf("between : %s and %s\n", clause_to_string(&node1->value, hash_table), clause_to_string(&node2->value, hash_table));
                        return true; // if the resolvent is empty, we have found a contradiction
                    }
                } else
                { // if the resolvent is already in the list ignore it
                    free(resolvent_node);
                    if (resolvent.literals != NULL) free (resolvent.literals); // free the memory allocated for literals
                    resolvent.literals = NULL; // set literals to NULL
                }
            }
            node2 = node2->next; // move to the next clause in the list
        }
        node1 = node1->next; // move to the next clause in the list
    }
    // if we reach here, we have not found a contradiction
    printf("No contradiction found, resolution by refutation failed\n");
    return false;
}