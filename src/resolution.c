#include <resolution.h>

#include "clauses_avl.h"


clause clause_parse(char *str , propositions_hash_table *hash_table)
{
    clause c ;
    init_clause(&c); // initialize the clause
    char pr_v[MAX_VARIABLE_LENGTH]; // charachter and two digits for index
    //pr_v[3] = '\0'; // null-terminate the string
    bool pr_v_signe = true; // true for positive, false for negative
    int literal_index = 0;
    int i = 0; // index for the string
    while (str[i] != '\0' && str[i]!='\n' && literal_index<MAX_LITERALS)
    {
        if (str[i] == ' ')
        {
            i++;
            continue; // skip spaces
        }
        if (str[i] == '-')
        {
            pr_v_signe = false;
        } else if ((str[i] >= 'A' && str[i] <= 'Z') || (str[i]>='a' && str[i]<='z')) // check if character letter
        {
            pr_v[0] = str[i]; // store character
            i++;
            if (str[i] >= '0' && str[i] <= '9') // check if character is digit
            {
                for (int j=1;j<MAX_VARIABLE_LENGTH;j++)
                {
                    if (str[i] >= '0' && str[i] <= '9') {// check if character is digit
                        pr_v[j] = str[i++]; // store digit
                    } else {
                        pr_v[j] = '\0'; // terminate string if no digit
                        break; // break if no more digits
                    }
                }
                pr_v[MAX_VARIABLE_LENGTH-1] = '\0'; // ensure null-termination
            } else pr_v[1] = '\0'; // if no digit, terminate string
            char *buffer = malloc(MAX_VARIABLE_LENGTH * sizeof(char));
            int j = 0;
            while (pr_v[j] != '\0' && j < MAX_VARIABLE_LENGTH - 1) {
                buffer[j] = pr_v[j]; // copy the proposition to buffer
                j++;
            }
            buffer[j] = '\0'; // null-terminate the buffer
            int index = proposition_index(buffer, hash_table); // get index of the proposition
            if (index == -1) {
                fprintf(stderr, "Error: Hash table overfloww\n");
                return c; // if index is -1, hash table is full
            }
            clasue_insert_literal(&c,(pr_v_signe ? index : -index)); // insert the literal into the clause
            pr_v_signe = true;
        }
        i++;
    }
    return c;
}

int proposition_index(char *proposition, propositions_hash_table *hash_table)
{
    int index = 1; // start from 1 because 0 has no complement
    while (index < hash_table->size )
    {
        if (hash_table->table[index] != NULL && variable_compare(hash_table->table[index],proposition) == 0)
        {
            return index; // found the proposition
        }
        index++;
    }
    // if not found, add it to the hash table
    if (index < MAX_VARIABLES)
    {
        hash_table->table[index] = proposition;
        hash_table->size++;
        return index; // return the index of the new proposition
    }
    fprintf(stderr, "Hash table is full, cannot add more propositions\n");
    return -1; // hash table is full, cannot add more propositions
}

void init_clause(clause *c)
{
    c->literals = malloc(MAX_LITERALS * sizeof(int)); // allocate memory for literals
    if (c->literals == NULL)
    {
        fprintf(stderr, "Memory allocation failed while allocating literals ,init_clause\n");
        return;
    }
    c->size = 0; // initialize size to 0
}

bool clasue_insert_literal(clause *c, int literal)
{
    int index;
    if (clause_search_literal(c, literal, &index)) {
        return false; // literal already exists, no need to insert
    }
    if (c->size < MAX_LITERALS) {
        // Shift elements to the right to make space for the new literal
        for (int i = c->size; i > index; i--) {
            c->literals[i] = c->literals[i - 1];
        }
        c->literals[index] = literal; // insert the new literal
        c->size++; // increment size of the clause
        return true; // literal inserted successfully
    } else {
        fprintf(stderr, "Clause is full, cannot insert more literals\n");
        return false; // clause is full, cannot insert more literals
    }
}

bool clause_search_literal(clause *c, int literal, int *found_index)
{
    int left = 0, right = c->size - 1;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (c->literals[mid] == literal) {
            if (found_index) *found_index = mid;
            return true;
        } else if (c->literals[mid] < literal) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    if (found_index) *found_index = left; // insertion point
    return false;
}

char *clause_to_string(clause *c, propositions_hash_table *hash_table)
{
    if (c == NULL || c->size == 0) {
        return NULL; // return NULL if clause is empty
    }
    char *result = malloc(MAX_LINE_LENGTH);
    if (result == NULL) {
        fprintf(stderr, "Memory allocation failed while allocating result string\n");
        return NULL;
    }
    result[0] = '\0'; // initialize result string
    for (int i = 0; i < c->size; i++) {
        int literal = c->literals[i];
        char *proposition = hash_table->table[abs(literal)];
        if (proposition == NULL) {
            fprintf(stderr, "Proposition not found in hash table\n");
            free(result);
            return NULL;
        }
        if (literal < 0) {
            strcat(result, "-"); // add negation sign for negative literals
        }
        strcat(result, proposition); // append proposition to result
        if (i < c->size - 1) {
            strcat(result, "| "); // add or between literals
        }
    }
    strncat(result, "\0", 1);
    return result; // return the resulting string
}

void init_hash_table(propositions_hash_table *hash_table)
{
    hash_table->size = 1; // initialize size to 0
    hash_table->table = malloc(MAX_VARIABLES * sizeof(char *)); // allocate memory for hash table
    hash_table->table[0] = malloc(MAX_VARIABLE_LENGTH);
    hash_table->table[0] = "0"; // index 0 is reserved for empty clasue
}


bool resolve_by_refutaion(clause_list *list,clause_node *start, propositions_hash_table *hash_table)
{
    // craate an AVL tree to store clauses to perform search for already existing clauses efficiently
    TNoeud *root = NULL; // root of the AVL tree
    clause_node *clause_list = list->head; // pointer to the clause list
    clause_node *previous =NULL ; // pointers to the current and previous nodes in the list

    while (clause_list != NULL) // iterate through the list of clauses
    {
        // insert clause into the AVL tree
        clause *c = &clause_list->value; // get the clause from the list
        clause_node *next;
        next= clause_list->next;
        if (clause_is_tautology(&clause_list->value)||!inserer(c, &root))
        {
            // insert the clause into the AVL tree

            free(clause_list);
            if (previous == NULL) { // if this is the first clause in the list
                next=list->head = next; // set head to the next clause
            } else {
                previous->next = next; // set previous next to the next clause
            }
            list->size--; // decrement the size of the list
        } else previous = clause_list; // if insertion was successful, set previous to current clause
        clause_list = next; // move to the next clause in the list
    }
    clause_node *node = list->head; // reset node to the head of the list
    int clause_count=0;
    while (node != NULL)
    {
        node->c1 = clause_count++; // set c1 to the current clause count
        node = node->next;
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
    int c1,c2;
    if (node1 != NULL) {
        c1 = node1->c1-1;
        c2 =-1; // indices of the clauses that were used to create this clause
    }


    while (node1 != NULL)
    {
        //if (node1->c2 == -2) break;
        c1++; // increment c1 for each clause
        //node2 = node1->next;
        c2=-1;

            //c2 = c1; // set c2 to c1 for initial clauses
            //node2 = node1->next;
            node2 = list->head;
            c2= -1; // reset c2 for each new node1
        while (node2 != node1)
        {
            c2++;
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
                    //list->tail->next = CreerClauseNoeud(); // create a new clause node
                    list->tail->next = resolvent_node; // set the next pointer of the tail to the new node
                    list->tail = list->tail->next; // move the tail to the new node
                    //list->tail->value = resolvent; // set the value of the new node to the resolvent
                    //list->tail->next = NULL; // set the next pointer to NULL
                    list->size++; // increment the size of the list
                    Aff_clause_node(resolvent_node, c1, c2); // set c1 and c2 for the resolvent node
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
    //printf("No contradiction found, resolution by refutation failed\n");
    return false;
}
/* * Complete order relation between clauses to use in AVL tree
 * Function to compare two clauses
 * Returns a negative value if c1 < c2, a positive value if c1 > c2, and 0 if they are equal
 */
int clause_compare(clause *c1, clause *c2)
{
    int size = c1->size < c2->size ? c1->size : c2->size;
    for (int i = 0; i < size; i++) {
        if (c1->literals[i] != c2->literals[i]) {
            return c1->literals[i] - c2->literals[i]; // return difference if literals are not equal
        }
    }
    return c1->size - c2->size; // return difference in size if all literals are equal
}

/* * Function to resolve two clauses and return the resolvent
 * If the clauses can be resolved and the resolvent is useful, it returns true and fills the result clause
 * If they cannot be resolved or the resolvent is not useful, it returns false
 */
bool clause_resolvent(clause *c1, clause *c2, clause * result)
{
    if (c1 == NULL | c2 == NULL ) return false;
    init_clause(result); // initialize the result clause
    int i = 0, j = c2->size-1; // indices for the clauses
    int negation_count = 0; // count of negations found
    int negated_literal =0; // the negated literal found
    while (i<c1->size && j >= 0 )
    {
        while (-c1->literals[i] > c2->literals[j] && i<c1->size) // find the literal in c2 that is the negation of the literal in c1
        {
            i++;
        }
        while (-c1->literals[i] < c2->literals[j] && j>=0) // find the literal in c1 that is the negation of the literal in c2
        {
            j--;
        }
        if (i < c1->size && j >= 0 && -c1->literals[i] == c2->literals[j]) // if we found the negation
        {
            negated_literal = abs(c1->literals[i]); // store the negated literal
            negation_count++;
            i++; // move to the next literal in c1
            j--; // move to the previous literal in c2
        }
        if (negation_count > 1) // if we found more than one negation, the resolvent is not useful
        {
            free(result->literals); // free the memory allocated for literals
            result->literals = NULL; // set literals to NULL
            result->size = 0; // set size to 0
            return false; // return false, we cannot resolve
        }
    }
    if (negation_count == 0) // if we did not find any negation, we cannot resolve
    {
        free(result->literals); // free the memory allocated for literals
        result->literals = NULL; // set literals to NULL
        result->size = 0; // set size to 0
        return false; // return false, we cannot resolve
    } else if (negation_count == 1) // if we found one negation, we can resolve
    {
        i = 0;
        j=0;
        result->size = c1->size + c2->size - 2; // size of the resolvent is the sum of sizes of the clauses minus 2 (for the negated literals)
        int k;
        if (result->size==0)
        {
            result->literals[0] = 0; // if the resolvent is empty, set the first literal to 0
            return true; // return true, we resolved successfully
        }
        for (k=0;k < result->size & i<c1->size & j<c2->size;k++)
        {
            if (i>=c1->size || j>=c2->size) break; // if we reached the end of one of the clauses, break

            if (abs(c1->literals[i]) == negated_literal) i++;
            if (abs(c2->literals[j]) == negated_literal) j++;

            if (i>=c1->size || j>=c2->size) break; // if we reached the end of one of the clauses, break

            if (c1->literals[i] < c2->literals[j] )
            {
                result->literals[k] = c1->literals[i];
                i++; // move to the next literal in c1
            } else if (c1->literals[i] > c2->literals[j])
            {
                result->literals[k] = c2->literals[j];
                j++;
            } else {
                result->literals[k] = c1->literals[i];
                i++; // move to the next literal in c1
                j++; // move to the next literal in c2
                result->size--;
            }
        }
        while (i<c1->size )
        {
            if (abs(c1->literals[i])!=negated_literal)result->literals[k++] = c1->literals[i]; // copy the remaining literals from c1
            i++; // move to the next literal in c1
        }
        while (j<c2->size )
        {
            if (abs(c2->literals[j])!=negated_literal)result->literals[k++] = c2->literals[j]; // copy the remaining literals from c2
            j++; // move to the next literal in c2
        }
        return true; // return true, we resolved successfully
    }

}

void insert_clause_node(clause_list *list, clause_node *node)
{
    if (list->head == NULL) { // if the list is empty
        list->head = node; // set head to the new node
        list->tail = node; // set tail to the new node
    } else {
        list->tail->next = node; // append the new node to the end of the list
        list->tail = node; // update tail to the new node
    }
    node->next = NULL; // set the next pointer of the new node to NULL
    list->size++; // increment size of the list
}
void save_clauses_after_resolution(FILE *file, clause_list *list, propositions_hash_table *hash_table)
{
    int index = 0; // index for the clauses
    clause_node *node = list->head; // start with the head of the list
    while (node != NULL) { // iterate through the list
        if (node->c2 == -1) { // if c2 is -1, it means this is an initial clause
            fprintf(file, "%d,,%s\n", index++, clause_to_string(&node->value, hash_table)); // print the clause to the file
        } else {
            fprintf(file, "c%d,res(c%d;c%d),%s\n", index++, node->c1, node->c2, clause_to_string(&node->value, hash_table)); // print the resolvent clause with its origin
        }
        node = node->next; // move to the next node
    }
}



bool clause_is_tautology(clause *c)
{
    if (c == NULL || c->size == 0) {
        return false;
    }
    for (int i = 0; i < c->size; i++) {
        for (int j = i + 1; j < c->size; j++) {
            if (c->literals[i] == -c->literals[j]) {
                return true; // if there is a literal and its negation, it is a tautology
            }
        }
    }
    return false; // no tautology found
}
int variable_compare(char *a, char *b)
{
    int i = 0;
    while (a[i]!= '\0' && b[i] != '\0' && i<MAX_VARIABLE_LENGTH) {
        if (a[i] != b[i]) {
            return a[i] - b[i]; // return difference if characters are not equal
        }
        i++;
    }
    return a[i] - b[i];
}

void getVariable(char *input, int *i,char* buffer)
{
    int j = 0;
    while (input[*i] != '\0' && input[*i] != ' ' && input[*i] != '&' && input[*i] != '|' && input[*i] != '>' && input[*i] != '=' && input[*i] != '(' && input[*i] != ')') {
        buffer[j++] = input[(*i)++];
    }
    buffer[j] = '\0'; // null-terminate the string
    (*i)--; // decrement i to point to the last character read
}

void save_clauses_to_file(clause_list list, FILE *file, propositions_hash_table *hash_table)
{
    clause_node *node = list.head;
    while (node != NULL) {
        fprintf(file, "%s\n", clause_to_string(&node->value, hash_table)); // print the clause to the file
        node = node->next; // move to the next node
    }
}

void append_string(char *buffer, int *index, const char *src)
{
    while (*src) {
        buffer[(*index)++] = *src++;
    }
}