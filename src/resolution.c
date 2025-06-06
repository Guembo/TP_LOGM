#include <resolution.h>
#include <stddef.h>
bool pv_is_equal(propositional_variable pv1, propositional_variable pv2)
{
    return (pv1.name == pv2.name && pv1.index == pv2.index);
}

bool pl_is_equal(propositional_literal pl1, propositional_literal pl2)
{
    return (pv_is_equal(pl1.variable, pl2.variable) && pl1.sign == pl2.sign);
}
bool pl_is_negation(propositional_literal pl1 , propositional_literal pl2)
{
    return (pv_is_equal(pl1.variable,pl2.variable) && pl1.sign != pl2.sign);
}

propositional_variable pv_create(char name, int index)
{
    propositional_variable pv;
    pv.name = name;
    pv.index = index;
    return pv;
}
propositional_literal pl_create(propositional_variable pv, bool sign)
{
    propositional_literal pl;
    pl.variable = pv;
    pl.sign = sign;
    return pl;
}
clause clause_create(propositional_literal *literals, int size)
{
    clause c;
    c.literals = literals;
    c.size = size;
    return c;
}
clause clause_parse(char *str)
{
    clause c;
    char pr_v[4]; // charachter and two digits for index
    pr_v[3] = '\0'; // null-terminate the string
    bool pr_v_signe = true; // true for positive, false for negative
    c.literals = (propositional_literal *)malloc(MAX_LITERALS * sizeof(propositional_literal)); // allocate memory for literals
    c.size = 0; // initialize size to 0
    int literal_index = 0;
    int i = 0; // index for the string
    while (str[i] != '\0' && str[i]!='\n' && literal_index<MAX_LITERALS)
    {
        if (str[i] == ' ')
        {
            i++;
            continue; // skip spaces
        }
        if (str[i] == '~')
        {
            pr_v_signe = false;
        } else if ((str[i] >= 'A' && str[i] <= 'Z') || (str[i]>='a' && str[i]<='z')) // check if character letter
        {
            pr_v[0] = str[i]; // store character
            i++;
            if (str[i] >= '0' && str[i] <= '9') // check if character is digit
            {
                pr_v[1] = str[i]; // store first digit
                i++;
                if (str[i] >= '0' && str[i] <= '9') // check for second digit
                {
                    pr_v[2] = str[i]; // store second digit
                } else {
                    pr_v[2] = '\0'; // terminate string if no second digit
                }
            } else pr_v[1] = '\0'; // if no digit, terminate string
            propositional_variable pv = pv_create(pr_v[0], atoi(pr_v + 1)); // create propositional variable
            propositional_literal pl = pl_create(pv, pr_v_signe); // create propositional literal
            c.literals[literal_index++] = pl; // add literal to clause
            c.size++; // increment size of clause
            pr_v_signe = true; // reset sign for next literal
        }
        i++;
    }
    return c;
}

void clause_str(clause c)
{
    for (int i = 0; i < c.size; i++)
    {
        if (!c.literals[i].sign)
        {
            printf("~");
        }
        printf("%c%d ", c.literals[i].variable.name, c.literals[i].variable.index);
        if (i < c.size - 1)
        {
            printf("| ");
        }
    }
}