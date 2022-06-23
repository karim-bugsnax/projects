#include "power_of_two.h"
#include <stdio.h>

int main()
{
    for (unsigned long i = 0; i < 64; i++)
    {
        printf("%s", "power_of_two(");
        printf("%lu", i);
        printf("%s", ") = ");
        printf("%lu \n", power_of_two(i));
    }
    
    return 0;
}
