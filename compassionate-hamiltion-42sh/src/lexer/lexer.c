#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "lexer_helper.h"
#include "../token/token.h"

struct lexer *lexer_new(const char *input)
{
    struct lexer *lexer = malloc(sizeof(struct lexer));
    if (lexer == NULL)
        return NULL;
    lexer->input = input;
    lexer->pos = 0;
    lexer->current_tok = NULL;
    lexer->last_was_non_word = false;
    lexer->word_until_semicolon = false;
    lexer->force_word_after_for = false;
    lexer->need_closing_var_bracket = false;
    return lexer;
}

void lexer_free(struct lexer *lexer)
{
    free(lexer);
}

struct token *lexer_peek(struct lexer *lexer)
{
    bool last_was_non_word = lexer->last_was_non_word;
    bool word_until_semicolon = lexer->word_until_semicolon;
    bool force_word_after_for = lexer->force_word_after_for;
    bool need_closing_var_bracket = lexer->need_closing_var_bracket;

    size_t i = lexer->pos;
    struct token *t = build_next_token(lexer, &i);

    lexer->last_was_non_word = last_was_non_word;
    lexer->word_until_semicolon = word_until_semicolon;
    lexer->force_word_after_for = force_word_after_for;
    lexer->need_closing_var_bracket = need_closing_var_bracket;

    return t;
}

struct token *lexer_pop(struct lexer *lexer)
{
    lexer->current_tok = build_next_token(lexer, &lexer->pos);
    return lexer->current_tok;
}


