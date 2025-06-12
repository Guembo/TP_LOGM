#include <formule_to_cnf.h>
#include <stdlib.h>
#include <stdio.h>
void print_tree(node t, int depth,propositions_hash_table *hash_table) {
    if (!t) return;
    for (int i = 0; i < depth; i++) putchar(' ');
    switch (t->type) {
    case VARIABLE: {
            int idx = t->val.variable;
            if (idx < 0) {
                printf("-%s\n", hash_table->table[-idx]);
            } else {
                printf("%s\n",hash_table->table[idx] );
            }
            break;
    }
    case NOT:
        printf("NOT\n");
        print_tree(t->val.connector.rc, depth+2,hash_table);
        break;
    case AND:
        printf("AND\n");
        print_tree(t->val.connector.lc, depth+2,hash_table);
        print_tree(t->val.connector.rc, depth+2,hash_table);
        break;
    case OR:
        printf("OR\n");
        print_tree(t->val.connector.lc, depth+2,hash_table);
        print_tree(t->val.connector.rc, depth+2,hash_table);
        break;
    case IMPLIES:
        printf("IMPLIES\n");
        print_tree(t->val.connector.lc, depth+2,hash_table);
        print_tree(t->val.connector.rc, depth+2,hash_table);
        break;
    case EQUIVALENT:
        printf("EQUIV\n");
        print_tree(t->val.connector.lc, depth+2,hash_table);
        print_tree(t->val.connector.rc, depth+2,hash_table);
        break;
    default:
        printf("UNK('%c')\n", t->type);
        break;
    }
}



// Stack abstract machine
void creatStsack(stack *s){
    *s=NULL;
}
node getTop(stack s){
    return s->val;
}
int isEmpty(stack s){
    return s==NULL;
}
void push(stack* s,node n){
    stack new = malloc(sizeof(*s));
    new->val=n;
    new->next = NULL;
    if (*s==NULL){
        *s= new;
    } else {
        new->next = *s;
        *s=new;
    }
}
node pop(stack *s){
    node e =(*s)->val;
    stack newTop = (*s)->next;
    (*s) = newTop;
    return (e);
}

node formula_to_tree(char *formula, propositions_hash_table *hash_table)
{
    char *c;
    int i=0;
    node tr;
    node connector_node=NULL;
    node formula_node=NULL;
    stack s;
    creatStsack(&s);
    while(formula[i]!='\0')
    {
        if (formula[i] == ' ') { // skip spaces
            i++;
            continue;
        }
        allocateTree(&tr);
        if (isConnector(formula[i]))
        {
            tr->type=formula[i];
            connector_node = tr;
            if (isEmpty(s)||comparePriority(formula[i],(getTop(s))->type)==1||getTop(s)->type==OPEN_BRACKET){
                push(&s,tr);
                tr->val.connector.lc = formula_node;
                formula_node = NULL;
            }else {
                while (!isEmpty(s) &&comparePriority(getTop(s)->type,connector_node->type)!=-1&& getTop(s)->type!=OPEN_BRACKET){
                    getTop(s)->val.connector.rc = formula_node;
                    formula_node = pop(&s);
                }
                push(&s,tr);
                tr->val.connector.lc=formula_node;
            }
        } else if (formula[i] == NOT)
        {
            tr->type=formula[i];
            tr->val.connector.rc = NULL;
            tr->val.connector.lc = NULL;
            push(&s,tr);
        } else if (formula[i]==OPEN_BRACKET){
            tr->type = OPEN_BRACKET;
            push(&s,tr);
        }else if(formula[i]==CLOSE_BRACKET){
            free(tr);
            while(getTop(s)->type != OPEN_BRACKET ){
                connector_node=pop(&s);
                connector_node->val.connector.rc=formula_node;
                formula_node = connector_node;
            }
            connector_node=pop(&s);
            free(connector_node);
        }else {
            char *buffer = malloc(MAX_VARIABLE_LENGTH * sizeof(char));
            getVariable(formula, &i, buffer);
            tr->val.variable = proposition_index(buffer, hash_table);
            tr->type = VARIABLE;
            formula_node = tr;
        }
        i++;
    }
    while(!isEmpty(s)){;
        connector_node=pop(&s);
        connector_node->val.connector.rc = formula_node;
        formula_node= connector_node;
    }
    node root = connector_node;
    return root;
}

void tree_to_formula(node root,propositions_hash_table hash_table ,char *buffer,int *index,int formula_type)
{
    if (root == NULL) return; // if the node is NULL, return
    if (formula_type == LEFT_FORMULA) buffer[(*index)++] = OPEN_BRACKET; // add opening parenthesis for left formula
    if (root->type == VARIABLE)
    {
        char *proposition = hash_table.table[abs(root->val.variable)];
        if (root->val.variable < 0) buffer[(*index)++] = NOT; // add negation sign for negative literals
        for (int i = 0; proposition[i] != '\0'; i++) {
            buffer[(*index)++] = proposition[i]; // append proposition to buffer
        }
    } else
    {
        if (root->type == NOT) buffer[(*index)++] = '('; // add opening parenthesis for negation
        tree_to_formula(root->val.connector.lc, hash_table, buffer, index, LEFT_FORMULA); // recursively call for left child
        buffer[(*index)++] = root->type; // add the operator to the buffer
        tree_to_formula(root->val.connector.rc, hash_table, buffer, index, RIGHT_FORMULA); // recursively call for right child
    }
    if (formula_type == RIGHT_FORMULA) buffer[(*index)++] = CLOSE_BRACKET; // add closing parenthesis for right formula
}

void tree_to_formula_reduced_parentheses(node root,propositions_hash_table hash_table ,char *buffer,int *index,int formula_type,int parenthese)
{
    if (root == NULL) return; // if the node is NULL, return
    if (root->type == VARIABLE)
    {
        char *proposition = hash_table.table[abs(root->val.variable)];
        if (root->val.variable < 0) buffer[(*index)++] = NOT; // add negation sign for negative literals
        for (int i = 0; proposition[i] != '\0'; i++) {
            buffer[(*index)++] = proposition[i]; // append proposition to buffer
        }
    } else
    {
        int close=0;
        // Check if we need to open a parenthesis based on the operator's priority
        // if parenthes >0 then we need to open a parenthesis
        // if it is 0 then we need to open a parenthesis only if the operator is EQUIVALENT or IMPLIES because AND and OR are associative
        if (parenthese>0 || (parenthese==0 && (root->type == EQUIVALENT || root->type == IMPLIES)))
        {
            buffer[(*index)++] = '(';
            close =1 ; // set a close flag to true if we opened a parenthesis
        }
        parenthese = 0; // reset parenthese flag for children
        if (root->val.connector.lc&&isConnector(root->val.connector.lc->type)) parenthese =comparePriority(root->type,root->val.connector.lc->type);
        tree_to_formula_reduced_parentheses(root->val.connector.lc, hash_table, buffer, index, LEFT_FORMULA,parenthese); // recursively call for left child
        buffer[(*index)++] = root->type; // add the operator to the buffer
        parenthese = 0;
        if (root->val.connector.rc && isConnector(root->val.connector.rc->type)) parenthese = comparePriority(root->type, root->val.connector.rc->type);
        tree_to_formula_reduced_parentheses(root->val.connector.rc, hash_table, buffer, index, RIGHT_FORMULA,parenthese);
        if (close)
        {
            buffer[(*index)++] = ')';
        }
    }

}

void latex_tree_to_formula(node root, propositions_hash_table hash_table, char *buffer, int *index, int formula_type,int parenthese) {
    if (root == NULL) return;

    //if (formula_type == LEFT_FORMULA && parenthese) buffer[(*index)++] = '(';

    if (root->type == VARIABLE) {
        char *proposition = hash_table.table[abs(root->val.variable)];
        if (root->val.variable < 0) append_utf8(buffer, index, "\\neg ");
        for (int i = 0; proposition[i] != '\0'; i++) {
            buffer[(*index)++] = proposition[i];
        }
    } else {
        int close=0;
        // Check if we need to open a parenthesis based on the operator's priority
        // if parenthes >0 then we need to open a parenthesis
        // if it is 0 then we need to open a parenthesis only if the operator is EQUIVALENT or IMPLIES because AND and OR are associative
        if (parenthese>0 || (parenthese==0 && (root->type == EQUIVALENT || root->type == IMPLIES)))
        {
            buffer[(*index)++] = '(';
            close =1 ; // set close flag to true if we opened a parenthesis
        }
        parenthese = 0; // reset parenthese flag for the next operator
        if (root->val.connector.lc&&isConnector(root->val.connector.lc->type)) parenthese =comparePriority(root->type,root->val.connector.lc->type);
        latex_tree_to_formula(root->val.connector.lc, hash_table, buffer, index, LEFT_FORMULA,parenthese);

        // Insert operator in LaTeX
        switch (root->type) {
        case AND:        append_utf8(buffer, index, " \\land "); break;
        case OR:         append_utf8(buffer, index, " \\lor "); break;
        case IMPLIES:    append_utf8(buffer, index, " \\rightarrow "); break;
        case EQUIVALENT: append_utf8(buffer, index, " \\leftrightarrow "); break;
        case NOT :     append_utf8(buffer, index, " \\neg "); break;
        default:         buffer[(*index)++] = root->type; break; // fallback
        }

        parenthese = 0;
        if (root->val.connector.rc && isConnector(root->val.connector.rc->type)) parenthese = comparePriority(root->type, root->val.connector.rc->type);
        latex_tree_to_formula(root->val.connector.rc, hash_table, buffer, index, RIGHT_FORMULA,parenthese);
        if (close)
        {
            buffer[(*index)++] = ')';
        }
    }

    //if (formula_type == RIGHT_FORMULA && parenthese) buffer[(*index)++] = ')';
}


//tree nodes abstract machine
//
void allocateTree(node * n){
    *n = malloc(sizeof(**n));

    (*n)->val.connector.lc =NULL;
    (*n)->val.connector.rc =NULL;
}

int isConnector(char c)
{
    return (c=='&' || c=='|' || c=='>' || c=='=');
}
int comparePriority(char a, char b)
{
    if (a==b)return 0;
    switch( a)
    {
    case AND:
        if (b==NOT) return -1; // negation has the highest priority
        return 1;
    case OR:
        if(b==AND || b==NOT)return -1;
        return 1;
        case IMPLIES:
        if(b==AND || b==OR || b==NOT)return -1;
        return 1;
    case EQUIVALENT:
        return -1; // equivalence has the lowest priority
    case NOT:return 1; // negation has the highest priority
    }
}






void append_utf8(char *buffer, int *index, const char *utf8_char)
{
    while (*utf8_char) {
        buffer[(*index)++] = *utf8_char++;
    }
}

void formula_tree_remove_equivalences(node *root)
{
    if (*root == NULL || (*root)->type==VARIABLE) return; // if the node is NULL, return

    // Recursively remove equivalences from left and right children
    formula_tree_remove_equivalences(&((*root)->val.connector.lc));
    formula_tree_remove_equivalences(&((*root)->val.connector.rc));

    // If the current node is an equivalence, replace it with its children
    if ((*root)->type == EQUIVALENT) {
        // create new OR node to replace the equivalence
        node new_root ;
        allocateTree(&new_root);
        new_root->type = OR; // replace equivalence with conjunction

        // Creat left child as AND of left child and right child negated
        node left_child;
        allocateTree(&left_child);
        left_child->type = AND; // left child is conjunction
        left_child->val.connector.lc = (*root)->val.connector.lc; // left child is the left child of the equivalence
        left_child->val.connector.rc = (*root)->val.connector.rc;
        new_root->val.connector.lc = left_child;
        // now for AND right child With negation
        node right_child;
        allocateTree(&right_child);
        right_child->type = AND;
        node lc,rc;
        allocateTree(&lc);
        allocateTree(&rc);
        lc->type = NOT; // left child is negation
        rc->type = NOT; // right child is negation
        lc->val.connector.rc = formula_tree_duplicate((*root)->val.connector.lc); // left child is the left child of the equivalence
        rc->val.connector.rc = formula_tree_duplicate((*root)->val.connector.rc); // right child is the right child of the equivalence
        right_child->val.connector.lc = lc; // left child is negation of left child
        right_child->val.connector.rc = rc; // right child is negation of right child
        new_root->val.connector.rc = right_child; // right child is negation of right child
        // free the old equivalence node
        free(*root); // free the old equivalence node
        *root = new_root; // replace the old equivalence node with the new OR node
    }
}

void formula_tree_remove_implications(node *root)
{
    if (*root == NULL || (*root)->type == VARIABLE) return; // if the node is NULL, return

    // Recursively remove implications from left and right children
    formula_tree_remove_implications(&((*root)->val.connector.lc));
    formula_tree_remove_implications(&((*root)->val.connector.rc));

    // If the current node is an implication, replace it with equuivelent formula
    // P -> Q is equivalent to ~P | Q
    if ((*root)->type == IMPLIES) {
        // create new OR node to replace the implication
        node new_root;
        allocateTree(&new_root);
        new_root->type = OR; // replace implication with disjunction

        // Create left child as negation of left child
        node left_child;
        allocateTree(&left_child);
        left_child->type = NOT; // left child is negation
        left_child->val.connector.rc = (*root)->val.connector.lc; // left child is the negation of left child of the implication
        new_root->val.connector.lc = left_child;

        // Create right child as right child of the implication
        node right_child = (*root)->val.connector.rc; // right child is the right child of the implication
        new_root->val.connector.rc = right_child;

        // free the old implication node
        free(*root); // free the old implication node
        *root = new_root; // replace the old implication node with the new OR node
    }
}

// DeMorgan Laws are used to push negation down the tree
/*
 * The root tree must contains only AND, OR, NOT and VARIABLE nodes
 */
void formula_tree_push_negation(node *root)
{
    if ((*root) == NULL || (*root)->type == VARIABLE) return; // if the node is NULL or a variable, return
    // we process the root then its children
    node new_root;
    if ((*root)->type == NOT) {
        // If the current node is a negation, we need to push it down
        switch ((*root)->val.connector.rc->type)
        {
        case VARIABLE: // if the right child is a variable, we inverse its sign and remove the negation
            node child = (*root)->val.connector.rc; // get the right child
            child->val.variable = -(child->val.variable); // inverse the sign of the variable
            free(*root); // free the negation node
            *root = child; // replace the negation node with the variable node
            return;
        case NOT: // if the right child is NOT, we remove the negation and push negation down its children
            new_root =(*root)->val.connector.rc->val.connector.rc;
            free((*root)->val.connector.rc);// free the negation node
            free(*root); // free the negation node
            (*root) = new_root;
            formula_tree_push_negation(root);
            return;
        case OR: // if the right child is OR, we replace it AND then negate its children
        case AND:// if the right child is AND we replace it with OR then negate its children
            allocateTree(&new_root); // allocate memory for the new node
            new_root->type = ((*root)->val.connector.rc->type == OR) ? AND : OR; // set the type to AND if the right child is OR, otherwise set it to OR
            // create negation of the left child
            node left_child;
            allocateTree(&left_child); // allocate memory for the left child
            left_child->type = NOT; // set the type of the left child to NOT
            left_child->val.connector.rc = (*root)->val.connector.rc->val.connector.lc; // set the right child of the left child to the left child of the right child
            new_root->val.connector.lc = left_child; // set the left child of the new root to the left child
            // now we need to create the right child of the new root
            node right_child;
            allocateTree(&right_child); // allocate memory for the right child
            right_child->type = NOT; // set the type of the right child to NOT
            right_child->val.connector.rc = (*root)->val.connector.rc->val.connector.rc; // set the right child of the right child to the right child of the right child
            new_root->val.connector.rc = right_child; // set the right child of the new root to the right child
            // free the old negation node
            free((*root)->val.connector.rc); // free the right child of the negation node
            free(*root); // free the old negation node
            *root = new_root;
            formula_tree_push_negation(&new_root->val.connector.lc);
            formula_tree_push_negation(&new_root->val.connector.rc);
            return;
        }
    }
    // Recursively push negation down the left and right children
    formula_tree_push_negation(&((*root)->val.connector.lc));
    formula_tree_push_negation(&((*root)->val.connector.rc));
}
/*
 * The root tree must contains only AND, OR and VARIABLE nodes
 */
void formula_tree_distribute_disjunctions(node *root)
{
    if (*root == NULL || (*root)->type == VARIABLE) return; // if the node is NULL or a variable, return

    // we have to traverse the tree in post-order to ensure that we process the children before the parent
    formula_tree_distribute_disjunctions(&((*root)->val.connector.lc)); // recursively distribute disjunctions in left child
    formula_tree_distribute_disjunctions(&((*root)->val.connector.rc)); // recursively distribute disjunctions in right child
    // If the current node is an OR, we need to distribute it over ANDs
    if ((*root)->type == OR)
    {
        // If the left child is an AND, we need to distribute the OR over the AND
        if ((*root)->val.connector.lc->type == AND) {
            // Create a new AND node to replace the OR
            node new_root;
            allocateTree(&new_root);
            new_root->type = AND; // set the type to AND

            // Create left child as OR of left child of AND and right child of OR
            node left_child;
            allocateTree(&left_child);
            left_child->type = OR; // set the type to OR
            left_child->val.connector.lc = (*root)->val.connector.lc->val.connector.lc; // set the left child to the left child of the AND
            left_child->val.connector.rc = (*root)->val.connector.rc; // set the right child to the right child of the OR
            new_root->val.connector.lc = left_child; // set the left child of the new root to the left child

            // Create right child as OR of right child of AND and right child of OR
            node right_child;
            allocateTree(&right_child);
            right_child->type = OR; // set the type to OR
            right_child->val.connector.lc = (*root)->val.connector.lc->val.connector.rc; // set the left child to the right child of the AND
            right_child->val.connector.rc = formula_tree_duplicate((*root)->val.connector.rc); // set the right child to the right child of the OR
            new_root->val.connector.rc = right_child; // set the right child of the new root to the right child

            // free the old OR node
            free((*root)->val.connector.lc);
            free(*root); // free the old OR node
            *root = new_root; // replace the old OR node with the new AND node
            // we have to traverse the tree in post-order to ensure that we process the children before the parent
            formula_tree_distribute_disjunctions(&((*root)->val.connector.lc)); // recursively distribute disjunctions in left child
            formula_tree_distribute_disjunctions(&((*root)->val.connector.rc)); // recursively distribute disjunctions in right child
        }
        // If the right child is an AND, we need to distribute the OR over the AND
        else if ((*root)->val.connector.rc->type == AND)
        {
            // Create a new AND node to replace the OR
            node new_root;
            allocateTree(&new_root);
            new_root->type = AND; // set the type to AND

            // Create left child as OR of left child of OR and left child of AND
            node left_child;
            allocateTree(&left_child);
            left_child->type = OR; // set the type to OR
            left_child->val.connector.lc = (*root)->val.connector.lc; // set the left child to the left child of the OR
            left_child->val.connector.rc = (*root)->val.connector.rc->val.connector.lc; // set the right child to the left child of the AND
            new_root->val.connector.lc = left_child; // set the left child of the new root to the left child
            // Create right child as OR of right child of OR and left child of AND
            node right_child;
            allocateTree(&right_child); // allocate memory for the right child
            right_child->type = OR; // set the type to OR
            right_child->val.connector.lc = formula_tree_duplicate((*root)->val.connector.lc); // set the left child to the right child of the OR
            right_child->val.connector.rc = (*root)->val.connector.rc->val.connector.rc; // set the right child to the right child of the AND
            new_root->val.connector.rc = right_child; // set the right child of the new root to the right child
            // free the old OR node
            free(*root); // free the old OR node
            *root = new_root; // replace the old OR node with the new AND node
            // we have to traverse the tree in post-order to ensure that we process the children before the parent
            formula_tree_distribute_disjunctions(&((*root)->val.connector.lc)); // recursively distribute disjunctions in left child
            formula_tree_distribute_disjunctions(&((*root)->val.connector.rc)); // recursively distribute disjunctions in right child
        }
    }
}

node formula_tree_duplicate(node root)
{
    if (root == NULL) return NULL; // if the node is NULL, return NULL
    if (root->type == VARIABLE) {
        node new_node;
        allocateTree(&new_node); // allocate memory for the new node
        new_node->type = VARIABLE; // set the type to VARIABLE
        new_node->val.variable = root->val.variable; // copy the variable value
        return new_node; // return the duplicated node
    }

    node new_node;
    allocateTree(&new_node); // allocate memory for the new node
    new_node->type = root->type; // copy the type of the node
    new_node->val.variable = root->val.variable; // copy the variable value

    // Recursively duplicate left and right children
    new_node->val.connector.lc = formula_tree_duplicate(root->val.connector.lc);
    new_node->val.connector.rc = formula_tree_duplicate(root->val.connector.rc);

    return new_node; // return the duplicated node
}

void destroy_tree(node *root)
{
    if (root==NULL|*root == NULL) return; // if the node is NULL, return
    // Recursively destroy left and right children
    destroy_tree(&((*root)->val.connector.lc));
    destroy_tree(&((*root)->val.connector.rc));
    free(*root); // free the current node
    *root = NULL; // set the pointer to NULL
}

node formula_to_fnc(char *formula, propositions_hash_table *hash_table, clause_list *list ,char *results[6])
{
    list->tail = NULL; // initialize the tail of the list to NULL
    list->head = NULL; // initialize the head of the list to NULL
    list->size = 0; // initialize the size of the list to 0



    char *buffer = malloc(MAX_LINE_LENGTH * sizeof(char)); // allocate memory for the buffer
    int index = 0; // index for the buffer

    node root = formula_to_tree(formula, hash_table); // convert formula to tree
    print_tree(root, 0, hash_table); // print the tree for debugging
    latex_tree_to_formula(root, *hash_table, results[0], &index, FIRST_FORMULA, 0); // convert tree to LaTeX formula
    results[0][index] = '\0'; // null-terminate the string
    index=0; // reset index for the buffer
    formula_tree_remove_equivalences(&root); // remove equivalences
    print_tree(root, 0, hash_table); // print the tree for debugging
    printf("=====\n");
    latex_tree_to_formula(root, *hash_table, results[1], &index, FIRST_FORMULA, 0);
    results[1][index] = '\0'; // null-terminate the string
    index = 0; // reset index for the buffer
    formula_tree_remove_implications(&root); // remove implications
    print_tree(root, 0, hash_table); // print the tree for debugging
    printf("=====\n");
    latex_tree_to_formula(root, *hash_table, results[2], &index, FIRST_FORMULA, 0); // convert tree to LaTeX formula
    results[2][index] = '\0'; // null-terminate the string
    index = 0; // reset index for the buffer
    formula_tree_push_negation(&root); // push negation down the tree
    print_tree(root, 0, hash_table); // print the tree for debugging
    printf("=====\n");
    //print_tree(root, 0, hash_table); // print the tree for debugging
    latex_tree_to_formula(root, *hash_table, results[3], &index, FIRST_FORMULA, 0); // convert tree to LaTeX formula

    results[3][index] = '\0'; // null-terminate the string
    index = 0; // reset index for the buffer
    formula_tree_distribute_disjunctions(&root); // distribute disjunctions
    print_tree(root, 0, hash_table); // print the tree for debugging
    printf("=====\n");
    //print_tree(root, 0, hash_table); // print the tree for debugging
    latex_tree_to_formula(root, *hash_table, results[4], &index, FIRST_FORMULA, 0); // convert tree to LaTeX formula
    results[4][index] = '\0'; // null-terminate the string
    index = 0; // reset index for the buffer
    buffer[index] = '\0'; // null-terminate the buffer
    results[5]=buffer;
    tree_to_formula_reduced_parentheses(root, *hash_table, buffer, &index, FIRST_FORMULA, 0); // convert tree to formula
    //destroy_tree(&root); // destroy the tree to free memory
    // Now we have a tree in CNF form, we need to convert it to a list of clauses and remove duplicates
    buffer[index] = '\0';
    char *clause_buffer = malloc(MAX_LINE_LENGTH * sizeof(char)); // allocate memory for the clause buffer
    int clause_index = 0; // index for the clause buffer
    TNoeud *avl_root = NULL; // root of the AVL tree
    int c1=0; // counter for clauses
    int c2 =-1;
    index=0;
    while (buffer[index] != '\0')
    {
        if (buffer[index]== ' '|| buffer[index] == '&' || buffer[index] == ')') {
            index++; // skip spaces
            continue;
        }else{
            clause_index = 0; // reset clause index
            while (buffer[index] != ')' && buffer[index] != '\0' && buffer[index] != '&'&&buffer[index] != ' ')
            {
                clause_buffer[clause_index++] = buffer[index++]; // copy the clause to the clause buffer
            }
            clause_buffer[clause_index] = '\0'; // null-terminate the clause buffer
            clause_node *new_clause = malloc(sizeof(clause_node));
            new_clause->value = clause_parse(clause_buffer,hash_table);
            if (!inserer(&new_clause->value, &avl_root)) {
                // if the clause is already in the AVL tree (duplicat, skip it
                free(new_clause); // free the clause node
                continue;
            } else
            {
                Aff_clause_node(new_clause,c1,c2);
                c1++; // increment the counter for clauses
                insert_clause_node(list, new_clause);
            }
        }

    }
    return root; // return the root of the tree
}

node clause_to_formula_tree(clause c)
{
    int *literals = c.literals;
    int size = c.size;
    if (size == 0) return NULL; // if the clause is empty, return NULL
    node root = NULL;
    node current = NULL;
    for (int i = 0; i < size; i++) {
        allocateTree(&current);
        current->type = VARIABLE;
        current->val.variable = literals[i];
        if (root == NULL) {
            root = current; // if the root is NULL, set it to the current node
        } else {
            node new_root;
            allocateTree(&new_root); // allocate memory for the new root
            new_root->type = OR; // set the type of the new root to OR
            new_root->val.connector.lc = root; // set the left child of the new root to the root
            new_root->val.connector.rc = current; // set the right child of the new root to the current node
            root = new_root; // set the root to the new root
        }
    }
}
int formula_tree_evaluate(node root, propositions_hash_table *hash_table, int *values)
{
    if (root == NULL) return 0; // if the node is NULL, return 0
    switch (root->type) {
    case VARIABLE: {
        int idx = root->val.variable;
        if (idx < 0) return !values[-idx]; // if the variable is negative, return the negation of the value
        return values[idx]; // return the value of the variable
    }
    case NOT:
        return !formula_tree_evaluate(root->val.connector.rc, hash_table, values); // evaluate the right child and negate it
    case AND:
        return formula_tree_evaluate(root->val.connector.lc, hash_table, values) && formula_tree_evaluate(root->val.connector.rc, hash_table, values); // evaluate both children and return their conjunction
    case OR:
        return formula_tree_evaluate(root->val.connector.lc, hash_table, values) || formula_tree_evaluate(root->val.connector.rc, hash_table, values); // evaluate both children and return their disjunction
    case IMPLIES:
        return !formula_tree_evaluate(root->val.connector.lc, hash_table, values) || formula_tree_evaluate(root->val.connector.rc, hash_table, values); // evaluate implication
    case EQUIVALENT:
        return formula_tree_evaluate(root->val.connector.lc, hash_table, values) == formula_tree_evaluate(root->val.connector.rc, hash_table, values); // evaluate equivalence
    default:
        return 0; // unknown type
    }
}
void formula_tree_truth_table(node root, propositions_hash_table *hash_table,char **truth_table)
{
    int *values = malloc(hash_table->size-1 * sizeof(int)); // allocate memory for the values
    int lines_count = 1 << (hash_table->size-1); // calculate the number of lines in the truth table = 2^(number of variables)
    int columns_count = hash_table->size; // calculate the number of columns in the truth table
    *truth_table = malloc(lines_count * sizeof(char));
    for (int i = 0; i < lines_count; i++) {
        for (int j = 1; j < columns_count; j++) {
            values[j] = (i >> j) & 1; // set the value to 1 if the j-th bit of i is set, otherwise set it to 0
            // this line let us generate all combinations of truth values for the variables
        }
        // Evaluate the formula with the current combination of truth values
        int result = formula_tree_evaluate(root, hash_table, values);
        (*truth_table)[i] = result ? 'T' : 'F'; // set the last column to 'T' if the result is true, otherwise set it to 'F'
    }
    //free(values); // free the allocated memory for the values
}
