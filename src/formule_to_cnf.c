#include <formule_to_cnf.h>

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

node formula
