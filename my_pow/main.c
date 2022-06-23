#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern int my_add(int a, int b);
extern void my_pow(int n);
extern void reverse_print(const char *str);
//extern size_t my_strlen(const char *s);
extern char *my_strcpy(char *dest, const char *source);

int main(void)
{

    printf("reverse_print:\n");
    char *s = "Hello world!";
    reverse_print(s);
    printf("\n");

    /*printf("strlen:\n");
    printf("%ld %ld\n", strlen(s), my_strlen(s));

    printf("strcpy:\n");
    char *out = malloc(sizeof(char) * 20);
    printf("%s~%s~%s\n", s, my_strcpy(out, s), out);
    free(out);*/

    return 0;
}