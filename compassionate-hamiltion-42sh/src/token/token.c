#include "token.h"

#include <err.h>
#include <stdlib.h>

#include "../utils/alloc.h"

struct token *token_new(enum token_type type)
{
    struct token *new = zalloc(sizeof(struct token));
    new->type = type;
    new->word = NULL;
    return new;
}

void token_free(struct token *token)
{
    if (!token)
        return;
    if (token->type == WORD || token->type == VAR_ASSIGN
     || token->type == RED_L || token->type == RED_R || token->type == ION)
        free(token->word);
    free(token);
}
