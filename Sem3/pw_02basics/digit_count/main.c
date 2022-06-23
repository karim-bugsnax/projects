#include "digit_count.h"
#include <stdio.h>


int main()
{
    unsigned long m = 2;
    printf("%s\n", "digit_count(0) = 1 ");
    printf("%s\n", "digit_count(1) = 1 ");

    for (unsigned long i = 0; i < 63; i++)
    {
        printf("%s", "digit_count(");
        printf("%lu", m);
        printf("%s", ") = ");
        printf("%u \n", digit_count(m));
        m = m*2;
    }
    return 0;
    
}
