#include "alloc.h"
#include "token.h"

/**
 * \page Lexer
 *
 * The lexer cuts some input text into block called tokens.

 * This process is done **on demand**: the lexer doesn't read the
 * input more than it needs, and only creates tokens when lexer_peek
 * or lexer_pop is called, and no token is available.
 *
 * "2 + 3" will produce 3 tokens:
 *   - TOKEN_NUMBER { .value = 2 }
 *   - TOKEN_PLUS
 *   - TOKEN_NUMBER { .value = 3 }
 */

struct lexer
{
    const char *input; ///< The input data
    size_t pos; ///< The current offset inside the input data
    struct token *current_tok; ///< The next token, if processed
};

/**
 * \brief Creates a new lexer given an input string.
 */
struct lexer *lexer_new(const char *input)
{
    struct lexer *lexer = xmalloc(sizeof(struct lexer));
    lexer->input = input;
    lexer->pos = 0;
    lexer->current_tok = NULL;
    return lexer;
}

/**
 ** \brief Free the given lexer, but not its input.
 */
void lexer_free(struct lexer *lexer)
{
    free(lexer);
}

// Aux Functions for Peek and Pop
char *cut_out_segment(const char *in, int start, int end)
{
    int len = end - start + 1;
    char *tmp = malloc(sizeof(char) * len);
    if (tmp == NULL)
        return NULL;
    for (int i = 0; i < len; i++)
        tmp[i] = in[i + start];
    tmp[len - 1] = '\0';
    return tmp;
}

struct token *build_next_token(struct lexer *lexer, size_t *index)
{
    struct token *t = NULL;
    switch(lexer->input[*index])
    {
    case '+':
        t = token_new(TOKEN_PLUS);
        break;
    case '-':
        t = token_new(TOKEN_MINUS);
        break;
    case '*':
        t = token_new(TOKEN_MUL);
        break;
    case '/':
        t = token_new(TOKEN_DIV);
        break;
    case '(':
        t = token_new(TOKEN_LEFT_PAR);
        break;
    case ')':
        t = token_new(TOKEN_RIGHT_PAR);
        break;
    case '\0':
        t = token_new(TOKEN_EOF);
        break;
    default:
        break;
    }

    if (t != NULL)
        *index += 1;
    // not an operator (number, eof, or error)   
    else //(t == NULL)
    {
        size_t new_pos = *index;
        while ('0' <= lexer->input[new_pos] && lexer->input[new_pos] <= '9')
            new_pos++;

        // not a number
        if (new_pos == *index)
            t = token_new(TOKEN_ERROR);
        else // convert number
        {
            char *num = cut_out_segment(lexer->input, *index, new_pos);
            t = token_new(TOKEN_NUMBER);
            t->value = atoi(num);
            free(num);

            *index = new_pos;
        }
    }

    while (lexer->input[*index] == ' ')
        *index += 1;

    if (t->type != TOKEN_NUMBER)
        t->value = -1;

    return t;
}

/**
 * \brief Returns the next token, but doesn't move forward: calling lexer_peek
 * multiple times in a row always returns the same result. This functions is
 * meant to help the parser check if the next token matches some rule.
 */
struct token *lexer_peek(struct lexer *lexer)
{
    size_t i = lexer->pos;
    return build_next_token(lexer, &i);
}

/**
 * \brief Returns the next token, and removes it from the stream:
 *   calling lexer_pop in a loop will iterate over all tokens until EOF.
 */
struct token *lexer_pop(struct lexer *lexer)
{
    lexer->current_tok = build_next_token(lexer, &lexer->pos);
    return lexer->current_tok;
}