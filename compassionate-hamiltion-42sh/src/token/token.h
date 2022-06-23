#pragma once

#include <unistd.h>

enum token_type
{
    IF = 0,
    ELSE,
    ELIF,
    THEN,
    FI,
    SEMICOLON,
    BACK_SN,
    QUOTE,
    WORD,
    ERROR,
    TOK_EOF,
    AND,
    OR,
    PIPE,
    REDIRECTION,
    WHILE,
    UNTIL,
    FOR,
    DO,
    DONE,
    IN,
    NEGATION,
    VAR_ASSIGN,
    ION,
    RED_L,
    RED_R,
    CONTINUE,
    BREAK,
    UNSET,
    BRACK_L,
    BRACK_R,
    CURL_L,
    CURL_R,
    EXPORT,
    CD,
    EXIT,
    ECHOO,
    DOT,
};

struct token
{
    enum token_type type; ///< The kind of token
    char *word; ///< If the token is a number, its value
};

/**
 * \brief Allocate a new token
 */
struct token *token_new(enum token_type type);

/**
 * \brief Frees a token
 */
void token_free(struct token *token);
