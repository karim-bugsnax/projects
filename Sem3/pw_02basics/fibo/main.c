#include <stdio.h>
#include "fibo.h"

int main()
{
    printf("%s\n", "fibo(0) = 0");
    printf("%s\n", "fibo(1) = 1");
    for (int i = 2; i <= 10; i++)
    {
        printf("%s", "fibo(");
        printf("%d", i);
        printf("%s",") = ");
        printf("%lu\n",fibo(i));
    }
    printf("%s","... \n");

    for (int i = 81; i <= 90; i++)
    {
        printf("%s", "fibo(");
        printf("%d", i);
        printf("%s",") = ");
        printf("%lu\n",fibo(i));
    }
    return 0;
}
