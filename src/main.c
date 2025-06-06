#include <stdio.h>
#include <stdlib.h>

#include "resolution.h"

int main(void)
{
    printf("Hello, World!\n");
    printf("%d\n",atoi("\0"));
    clause_str(clause_parse("P5|Q|R2|H83\0"));
    return 0;
}
