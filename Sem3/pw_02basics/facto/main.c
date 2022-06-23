#include "facto.h"
#include <stdio.h>

int main()
{
    for (int i = 0; i <= 20; i++)
    {
        printf("%s", "facto(");
        printf("%d", i);
        printf("%s",") = ");
        printf("%lu\n",facto(i));
    }

    return 0;
}

