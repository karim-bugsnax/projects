#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "lexer_helper.h"
#include "../token/token.h"

// in this file are all auxilary functions for lexer_peek and lexer_pop
// cut_out_segment, check_var_syntax, is_delim, skip_comment, handle_backslash
//   is_brack_delim, handle_special_chars, handle_quote, get_token_type, build_next_token

// returns a copy of a segment of the input string
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

enum token_type check_var_syntax(char *tok)
{
    if (!isalpha(tok[0]))
        return WORD;
    int i;
    for (i = 1; tok[i] != '='; i++)
    {
        if (!(isalnum(tok[i]) || tok[i] == '_'))
            return WORD;
    }
    if (tok[i + 1] == '\0')
        return WORD;

    return VAR_ASSIGN;
}

// return true if c is a deliminating character
bool is_delim(char c)
{
    return c == ' ' || c == ';' || c == '\n' || c == '#' || c == '\0';
}

// loops until end of comment (\n)
void skip_comment(struct lexer *lexer, size_t *index)
{
    if (lexer->input[*index] == '#')
    {
        while (lexer->input[*index] != '\n' && lexer->input[*index] != '\0')
            *index += 1;
    }
}

void handle_backslash(struct lexer *lexer, size_t *new_pos)
{
    if (lexer->input[*new_pos + 1] == '$' || lexer->input[*new_pos + 1] == '\''
        || lexer->input[*new_pos + 1] == '\"' || lexer->input[*new_pos + 1] == '\\'
        || lexer->input[*new_pos + 1] == '\n' || lexer->input[*new_pos + 1] == '`'
        || lexer->input[*new_pos + 1] == '{' || lexer->input[*new_pos + 1] == '}')
        *new_pos += 1;
}

bool is_brack_delim(char c)
{
    return c == ' ' || c == ';' || c == '|' || c == '>' || c == '<' || c == '\t' || c == '&' || c == '\0';
}

// manages \0 ; \n cases
struct token *handle_special_chars(struct lexer *lexer, size_t *index)
{
    if (lexer->input[*index] == '\0')
        return token_new(TOK_EOF);
    
    if (lexer->input[*index] == ';')
    {
        if (lexer->force_word_after_for == true)
            return token_new(ERROR);
        lexer->word_until_semicolon = false;
        *index += 1;
        return token_new(SEMICOLON);
    }
    
    if (lexer->input[*index] == '\n')
    {
        lexer->word_until_semicolon = false;
        *index += 1;
        lexer->word_until_semicolon = false;
        return token_new(BACK_SN);
    }

    if (lexer->input[*index] == '&' && lexer->input[*index + 1] == '&')
    {
        *index += 2;
        lexer->word_until_semicolon = false;
        return token_new(AND);
    }

    if (lexer->input[*index] == '|')
    {
        if (lexer->input[*index + 1] == '|')
        {
            *index += 2;
            return token_new(OR);
        }

        *index += 1;
        return token_new(PIPE);
    }

    if ('0' <= lexer->input[*index] && lexer->input[*index] <= '2'
     && (lexer->input[*index + 1] == '<' || lexer->input[*index + 1] == '>'))
    {
        *index += 1;
        return token_new(ION);
    }

    if (lexer->input[*index] == '(')
    {
        *index += 1;
        return token_new(BRACK_L);
    }
    if (lexer->input[*index] == ')')
    {
        *index += 1;
        return token_new(BRACK_R);
    }
    if (lexer->input[*index] == '{' && is_brack_delim(lexer->input[*index + 1]))
    {
        *index += 1;
        return token_new(CURL_L);
    }
    if (lexer->input[*index] == '}')
    {
        if (*index != 0 && is_brack_delim(lexer->input[*index - 1]))
        {
            *index += 1;
            return token_new(CURL_R);
        }
    }

    return NULL;
}

// loops until next ' and handles error
int handle_quote(struct lexer *lexer, size_t *index, size_t *new_pos, char quote_symbol)
{
    index = index;
    int type = -1;
    if (*new_pos != 0 && lexer->input[*new_pos - 1] == '=')
        type = VAR_ASSIGN;
    if (lexer->force_word_after_for == true)
    {
        lexer->force_word_after_for = false;
        type = WORD;
    }

    *new_pos += 1;
    while(lexer->input[*new_pos] != quote_symbol)// || lexer->input[new_pos - 1] == '\\')
    {
        if (lexer->input[*new_pos] == '\0')
            return -2;

        if (lexer->input[*new_pos] == '\\' && quote_symbol != '\'')
            handle_backslash(lexer, new_pos);

        *new_pos += 1;
    }

    return type;
}

// returns the suitable token type for a given string
enum token_type get_token_type(struct lexer *lexer, char *tok)
{
    if (lexer->word_until_semicolon == true)
        return WORD;
    if (lexer->force_word_after_for == true)
    {
        lexer->force_word_after_for = false;
        return WORD;
    }

    enum token_type type;
    
    if (strcmp(tok, "if") == 0)
        type = IF;
    else if (strcmp(tok, "else") == 0)
        type = ELSE;
    else if (strcmp(tok, "elif") == 0)
        type = ELIF;
    else if (strcmp(tok, "then") == 0)
        type = THEN;
    else if (strcmp(tok, "fi") == 0)
        type = FI;
    else if (strcmp(tok, "while") == 0)
        type = WHILE;
    else if (strcmp(tok, "until") == 0)
        type = UNTIL;
    else if (strcmp(tok, "for") == 0)
    {
        lexer->force_word_after_for = true;
        type = FOR;
    }
    else if (strcmp(tok, "do") == 0)
        type = DO;
    else if (strcmp(tok, "done") == 0)
        type = DONE;
    else if (strcmp(tok, "in") == 0)
    {
        lexer->force_word_after_for = true;
        type = IN;
    }
    else if (strcmp(tok, "!") == 0)
        type = NEGATION;
    else if (strcmp(tok, ">") == 0 || strcmp(tok, ">&") == 0
     || strcmp(tok, ">>") == 0 || strcmp(tok, ">|") == 0)
        type = RED_R;
    else if (strcmp(tok, "<") == 0 || strcmp(tok, "<&") == 0
        || strcmp(tok, "<>") == 0)
        type = RED_L;
    else if (strcmp(tok, "continue") == 0)
        type = CONTINUE;
    else if (strcmp(tok, "break") == 0)
        type = BREAK;
    else if (strcmp(tok, "unset") == 0)
        type = UNSET;
    else if (strcmp(tok, "cd") == 0)
    {
        lexer->force_word_after_for = true;
        type = CD;
    }
    else if (strcmp(tok, "exit") == 0)
        type = EXIT;
    else if (strcmp(tok, "export") == 0)
        type = EXPORT;
    else
    {
        // check if tok is a variable assignment
        int j;
        for (j = 0; tok[j] != '\0' && tok[j] != '='; j++);
        if (tok[j] == '=')
        {
            type = check_var_syntax(tok);
            if (type == WORD && lexer->last_was_non_word)
                lexer->word_until_semicolon = true;
        }
        else
        {
            type = WORD;
            if (lexer->last_was_non_word)
                lexer->word_until_semicolon = true;
        }
    }

    return type;
}

// main function of this file, builds the next token
struct token *build_next_token(struct lexer *lexer, size_t *index)
{
    skip_comment(lexer, index);
    struct token *t = handle_special_chars(lexer, index);
    
    if (t == NULL)
    {
        // loop until to find end of token (and handle ')
        size_t new_pos = *index;
        int quote_type = -1;
        while (!(is_delim(lexer->input[new_pos])
         || (lexer->input[new_pos] == '&' && lexer->input[new_pos + 1] == '&')
         || (lexer->input[new_pos] == '|' && lexer->input[new_pos - 1] != '>')))
        {
            if (lexer->input[new_pos] == '\'' || lexer->input[new_pos] == '\"'
                 || lexer->input[new_pos] == '`')
            {
                int tmp = handle_quote(lexer, index, &new_pos, lexer->input[new_pos]);
                if (tmp == -2)
                    return token_new(ERROR);
                if (tmp != -1 && quote_type != -1)
                    quote_type = tmp;
            }
            if (lexer->input[new_pos] == '\\')
                handle_backslash(lexer, &new_pos);

            // Handle { } ( )
            if (lexer->input[new_pos] == '{' || lexer->input[new_pos] == '}')
            {
                if (new_pos != 0 && lexer->input[new_pos] == '{' && lexer->input[new_pos - 1] == '$')
                    lexer->need_closing_var_bracket = true;

                if (lexer->input[new_pos] == '}' && lexer->need_closing_var_bracket == true)
                    lexer->need_closing_var_bracket = false;

                else if (new_pos == 0)
                {
                    if (is_brack_delim(lexer->input[new_pos + 1]))
                        break;
                }
                else if (is_brack_delim(lexer->input[new_pos - 1])
                 && is_brack_delim(lexer->input[new_pos + 1]))
                    break;
            }

            if (lexer->input[new_pos] == '(' && lexer->input[new_pos - 1] == '$')
                lexer->need_closing_var_bracket = true;
            if (lexer->input[new_pos] == '(' && lexer->input[new_pos - 1] != '$')
                break;
            
            if (lexer->input[new_pos] == ')' && lexer->need_closing_var_bracket == false)
                break;
            if (lexer->input[new_pos] == ')' && lexer->need_closing_var_bracket == true)
                lexer->need_closing_var_bracket = false;

            new_pos++;
        }

        // cut out token and check type
        char *tok = cut_out_segment(lexer->input, *index, new_pos);
        if (quote_type == -1)
            t = token_new(get_token_type(lexer, tok));
        else
            t = token_new(quote_type);

        // free if necessary or set
        if (t->type == WORD || t->type == VAR_ASSIGN || t->type == RED_R || t->type == RED_L || t->type == ION)
            t->word = tok;
        else
            free(tok);

        *index = new_pos;
    }
 
    // skip spaces
    while (lexer->input[*index] == ' ' || lexer->input[*index] == '\t')
        *index += 1;

    // set last_was_non_word
    lexer->last_was_non_word = true;
    if (t->type == WORD || t->type == RED_R || t->type == RED_L || t->type == ION)
        lexer->last_was_non_word = false;

    return t;
}

