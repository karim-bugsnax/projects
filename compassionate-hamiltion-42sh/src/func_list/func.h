#pragma once

struct func
{
    char *name;
    int defined;
    struct ast *ast;
    struct func *next;
};

struct func *func_new(char *name);
struct func *func_add(struct func *list, struct func *new);
void func_empty(struct func *list);
void func_free(struct func *func);

