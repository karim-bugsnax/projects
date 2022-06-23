#include "isqrt.h"
#include <stdio.h>

int main() {
    int i = 0;
    while (i<= 200)
    {   
        printf("%s", "isqrt(");
        printf("%d", i);
        printf("%s", ") = ");
        printf("%lu \n", isqrt(i));
        i+=8;
    }
    return 0;
    
}
