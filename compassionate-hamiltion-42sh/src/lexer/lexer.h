#pragma once

#include <stdbool.h>

#include "../token/token.h"

struct lexer
{
    const char *input; ///< The input data
    size_t pos; ///< The current offset inside the input data
    struct token *current_tok; ///< The next token, if processed
    bool last_was_non_word;
    bool word_until_semicolon; ///< to force words after a command until a semicolon
    bool force_word_after_for; ///< to force a word after a for
    bool need_closing_var_bracket; ///< to identify if a ) is of $(cmd) or (true||false)
};

struct lexer *lexer_new(const char *input);
void lexer_free(struct lexer *lexer);
struct token *lexer_peek(struct lexer *lexer);
struct token *lexer_pop(struct lexer *lexer);