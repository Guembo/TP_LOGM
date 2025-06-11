#include <formule_to_cnf.h>
#include <stdlib.h>
#include <stdio.h>

int main(void)
{
    char* formula = "-(P&Q)|C>P=P>Q>C\0";
    propositions_hash_table hash_table;
    init_hash_table(&hash_table);
    node root = formula_to_tree(formula, &hash_table);
    char buffer[MAX_LINE_LENGTH];
    int index = 0;
    tree_to_formula(root, hash_table, buffer, &index, FIRST_FORMULA);
    buffer[index] = '\0'; // null-terminate the string
    printf("Converted formula: %s\n", buffer);
    index = 0;
    latex_tree_to_formula(root, hash_table, buffer, &index, FIRST_FORMULA);
    buffer[index] = '\0'; // null-terminate the string

    printf("Latex Code formula: %s\n", buffer);
    return 0;
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

void latex_tree_to_formula(node root, propositions_hash_table hash_table, char *buffer, int *index, int formula_type) {
    if (root == NULL) return;

    if (formula_type == LEFT_FORMULA) buffer[(*index)++] = '(';

    if (root->type == VARIABLE) {
        char *proposition = hash_table.table[abs(root->val.variable)];
        if (root->val.variable < 0) append_utf8(buffer, index, "\\neg ");
        for (int i = 0; proposition[i] != '\0'; i++) {
            buffer[(*index)++] = proposition[i];
        }
    } else {
        latex_tree_to_formula(root->val.connector.lc, hash_table, buffer, index, LEFT_FORMULA);

        // Insert operator in LaTeX
        switch (root->type) {
        case AND:        append_utf8(buffer, index, " \\land "); break;
        case OR:         append_utf8(buffer, index, " \\lor "); break;
        case IMPLIES:    append_utf8(buffer, index, " \\rightarrow "); break;
        case EQUIVALENT: append_utf8(buffer, index, " \\leftrightarrow "); break;
        case NOT :     append_utf8(buffer, index, " (\\neg "); break;
        default:         buffer[(*index)++] = root->type; break; // fallback
        }

        latex_tree_to_formula(root->val.connector.rc, hash_table, buffer, index, RIGHT_FORMULA);
    }

    if (formula_type == RIGHT_FORMULA) buffer[(*index)++] = ')';
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

void getVariable(char *input, int *i,char* buffer)
{
    int j = 0;
    while (input[*i] != '\0' && input[*i] != ' ' && input[*i] != '&' && input[*i] != '|' && input[*i] != '>' && input[*i] != '=' && input[*i] != '(' && input[*i] != ')') {
        buffer[j++] = input[(*i)++];
    }
    buffer[j] = '\0'; // null-terminate the string
    (*i)--; // decrement i to point to the last character read
}

void init_hash_table(propositions_hash_table *hash_table)
{
    hash_table->size = 1; // initialize size to 0
    hash_table->table = malloc(MAX_VARIABLE_LENGTH * sizeof(char *)); // allocate memory for hash table
    hash_table->table[0] = malloc(MAX_VARIABLE_LENGTH);
    hash_table->table[0] = "0"; // index 0 is reserved for empty clasue
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
    formula_tree_remove_equivalences(&(*root)->val.connector.lc);
    formula_tree_remove_equivalences(&(*root)->val.connector.rc);

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

    }
}

node formula_tree_duplicate(node root)
{
    if (root == NULL) return NULL; // if the node is NULL, return NULL

    node new_node;
    allocateTree(&new_node); // allocate memory for the new node
    new_node->type = root->type; // copy the type of the node
    new_node->val.variable = root->val.variable; // copy the variable value

    // Recursively duplicate left and right children
    new_node->val.connector.lc = formula_tree_duplicate(root->val.connector.lc);
    new_node->val.connector.rc = formula_tree_duplicate(root->val.connector.rc);

    return new_node; // return the duplicated node
}