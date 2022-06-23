#include "ast.h"

#include <err.h>
#include <stdlib.h>

#include "../utils/alloc.h"

struct ast *ast_new(enum ast_type type)
{
    struct ast *new = malloc(sizeof(struct ast));
    new->children = malloc(sizeof(struct ast *));
    new->cmd.cmd_arr = NULL;
    new->cmd.index = 0;
    new->cmd.cmd = NULL;
    new->cmd.args = NULL;
    new->cmd.type = 0;
    new->cmd.empty = 1;
    new->cond = 0;
    new->nb_children = 0;
    new->negation = 0;
    new->fd = -1;
    new->type = type;
    new->cont = 0;
    new->brk = 0;
    return new;
}

void ast_free(struct ast *ast)
{
    if (ast == NULL)
        return;

    for (size_t i = 0; i < ast->nb_children ; i++)
        ast_free(ast->children[i]);

    free(ast->cmd.cmd);
    free(ast->cmd.args);

    //need to free array of array
    for (size_t j = 0; j < ast->cmd.index ; j++)
        free(ast->cmd.cmd_arr[j]);
    free(ast->cmd.cmd_arr);

    free(ast->children);
    free(ast);
}
