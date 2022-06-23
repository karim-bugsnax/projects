#include "stdio.h"
#include "stdlib.h"
#include "mix.h"

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("%s\n", "This is the default text.");
        printf("%s\n", "Tihs is the dfeualt txet.");
        return 0;
    }
    mix(argv[1]);
    return 0;
}

