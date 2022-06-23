#include "func.h"
#include "../ast/ast.h"
#include <err.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>

struct func *func_new(char *name)
{
    struct func *new = malloc(sizeof(struct func));

    new->name = strdup(name);
    new->defined = 0;
    new->ast = NULL;
    new->next = NULL;

    return new;
}

struct func *func_add(struct func *list, struct func *new)
{
    new->next = list;
    return new;
}

void func_free(struct func *func)
{
    if (!func)
        return;
    free(func->name);
    ast_free(func->ast);
    free(func->next);
}

void func_empty(struct func *list)
{

    while(list)
    {
        struct func *tmp = list;
        list  = list->next;
        func_free(tmp);
    }
}
struct func *func_find(struct func *list, char *name)
{

    while(list)
    {
        if (!strcmp(list->name, name))
            return list;
        list  = list->next;
    }
    return NULL;
}
