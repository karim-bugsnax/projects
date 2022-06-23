#include "divisor_sum.h"
#include <stdio.h>
#include <err.h>
#include "stdlib.h"

int main(int argc, char** argv){
    unsigned long param = strtoul(argv[1], NULL, 10);
    if (argc != 2 || param == 0)
    {
        errx(1, "Error");
    }
    else
    {
        printf("%s", "divisor_sum(");
        printf("%s", argv[1]);
        printf("%s", ") = ");
        printf("%lu \n", divisor_sum(param));
    }
    return 0;
    
}