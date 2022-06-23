#include "parser.h"
#include "../ast/ast.h"
#include "../lexer/lexer.h"
#include "../func_list/func.h"
#include <err.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <ctype.h>


//static enum parser_status parse_func(struct ast **ast, struct lexer *lexer);
static enum parser_status parse_list(struct ast **ast, struct lexer *lexer);
static enum parser_status parse_compound_list(struct ast **ast, struct lexer *lexer);
static enum parser_status parse_and_or(struct ast **ast, struct lexer *lexer);
static enum parser_status parse_rule_if(struct ast **ast, struct lexer *lexer);
enum parser_status parse_input(struct ast **ast, struct lexer *lexer, struct func **main_list);
static enum parser_status parse_simple_cmd(struct ast **ast, struct lexer *lexer);
static enum parser_status parse_shell_cmd(struct ast **ast, struct lexer *lexer);
static enum parser_status parse_cmd(struct ast **ast, struct lexer *lexer);
static enum parser_status parse_pipeline(struct ast **ast, struct lexer *lexer);
static enum parser_status parse_else_clause(struct ast **ast,
                                            struct lexer *lexer);
static enum parser_status parse_rule_for(struct ast **child, struct lexer *lexer);
static enum parser_status parse_rule_while(struct ast **child, struct lexer *lexer);
static enum parser_status parse_rule_until(struct ast **child, struct lexer *lexer);
static enum parser_status parse_rule_do_group(struct ast **child, struct lexer *lexer);
static enum parser_status parse_prefix(struct ast **child, struct lexer *lexer);
static enum parser_status parse_element(struct ast **child, struct lexer *lexer);
static enum parser_status parse_redirection(struct ast **child, struct lexer *lexer);

static struct func *list = NULL;


/*static enum parser_status handle_parse_error(enum parser_status status,
                                             struct ast **ast)
{
    warnx("unexpected token");
    ast_free(*ast);
    *ast = NULL;
    return status;
}*/

//get the type on the parsed commad, i.e if word is echo returns type echo
static enum cmd_type get_cmd_type(char *word)
{
    if (!strcmp(word, "echo"))
        return ECHO;
    return OTHER;
}



/**
 * \brief Parse either an expression, or nothing
 *
 * input:  EOF  |  exp EOF
 */
enum parser_status parse_input(struct ast **ast, struct lexer *lexer, struct func **main_list)
{
    list = *main_list;
    // If we're at the end of file, there's no input
    struct token *tok = lexer_peek(lexer);
    if (tok->type == BACK_SN)
    {
        token_free(tok);
        return PARSER_CONTINUE;
    }
    if (tok->type == TOK_EOF)
    {
        (*ast)->fd = 69;
        token_free(tok);
        return PARSER_OK;
    }


    // try to parse an expression. if an error occured, free the
    // produced ast and return the same error code
    enum parser_status status = parse_list(ast, lexer);
    if (status != PARSER_OK)
    {
        token_free(tok);
        return PARSER_UNEXPECTED_TOKEN;
    }
        //errx(2, "unexpected token (line 57)");

    *main_list = list;

    // once parsing the expression is done, we should have
    // reached the end of file or new line.
    token_free(tok);
    tok = lexer_peek(lexer);
    if (tok->type == BACK_SN)
    {
        token_free(tok);
        return PARSER_CONTINUE;
    }
    if (tok->type == TOK_EOF)
    {
        token_free(tok);
        return PARSER_OK;
    }
    token_free(tok);
    // if we didn't reach the end of file, it's an error
    //return handle_parse_error(PARSER_UNEXPECTED_TOKEN, ast);
    //errx(2, "unexpected token (line 66)");
    return PARSER_UNEXPECTED_TOKEN;

}

static enum parser_status parse_list(struct ast **ast, struct lexer *lexer)
{
    //call parse cmd
    enum parser_status status = parse_and_or(ast, lexer);
    if (status != PARSER_OK)
        return status;

    //loop to keep parsing commands while there is a semicolon
    struct token *tok = NULL;
    while (true)
    {
        tok = lexer_peek(lexer);
        if (tok->type != SEMICOLON)
            break;
        token_free(tok);
        token_free(lexer_pop(lexer));
        status = parse_and_or(ast, lexer);
        if (status != PARSER_OK)
        {
            tok = lexer_peek(lexer);
            if (tok->type != SEMICOLON && tok->type != BACK_SN &&
                    tok->type != TOK_EOF)
            {
                token_free(tok);
                return status;
            }
            token_free(tok);
        }
    }

    token_free(tok);
    return PARSER_OK;
}

static enum parser_status parse_cmd(struct ast **ast, struct lexer *lexer)
{
    struct token *tok = lexer_peek(lexer);
    struct ast *child = NULL;

    if (tok->type == IF || tok->type == WHILE || tok->type == FOR 
        || tok->type == UNTIL || tok->type == BRACK_L || tok->type == CURL_L)
    {
        enum parser_status status = parse_shell_cmd(ast, lexer);
        if (status != PARSER_OK)
        {
            token_free(tok);
            ast_free(child);
            return status;
        }
        //implemnt (redirection)* rule if we can
        /*while (true)
        {
            status = parse_redirection(ast, lexer);
            if (status != PARSER_OK)
                break;
        }*/
    }
    else if (tok->type == CONTINUE)
    {
        child = ast_new(AST_CONT);
        token_free(tok);
        token_free(lexer_pop(lexer));
        tok = lexer_pop(lexer);
        //check if we will have continue [n]
        if (tok->word != NULL && isdigit(tok->word[0]))
        {
            child->cont = atoi(tok->word);
        }
        else
            child->cont = 1;

        (*ast)->children = realloc((*ast)->children, ((*ast)->nb_children + 1) * sizeof(struct ast *));
        (*ast)->children[(*ast)->nb_children] = child;
        (*ast)->nb_children += 1;
    }
    else if (tok->type == BREAK)
    {
        child = ast_new(AST_BRK);
        token_free(tok);
        token_free(lexer_pop(lexer));
        tok = lexer_pop(lexer);
        if (tok->word != NULL && isdigit(tok->word[0]))
        {
            child->brk = atoi(tok->word);
        }
        else
            child->brk = 1;

        (*ast)->children = realloc((*ast)->children, ((*ast)->nb_children + 1) * sizeof(struct ast *));
        (*ast)->children[(*ast)->nb_children] = child;
        (*ast)->nb_children += 1;
    }
    else if (tok->type == EXPORT)
    {
        child = ast_new(AST_EXPORT);
        token_free(tok);
        token_free(lexer_pop(lexer));
        tok = lexer_peek(lexer);
        child->cmd.cmd = strdup(tok->word);
        token_free(tok);
        token_free(lexer_pop(lexer));
        (*ast)->children = realloc((*ast)->children, ((*ast)->nb_children + 1) * sizeof(struct ast *));
        (*ast)->children[(*ast)->nb_children] = child;
        (*ast)->nb_children += 1;
    }
    else if (tok->type == CD)
    {
        child = ast_new(AST_CD);
        token_free(tok);
        token_free(lexer_pop(lexer));
        tok = lexer_peek(lexer);
        child->cmd.cmd = strdup(tok->word);
        token_free(lexer_pop(lexer));
        (*ast)->children = realloc((*ast)->children, ((*ast)->nb_children + 1) * sizeof(struct ast *));
        (*ast)->children[(*ast)->nb_children] = child;
        (*ast)->nb_children += 1;
    }
    else if (tok->type == EXIT)
    {
        child = ast_new(AST_EXIT);
        token_free(lexer_pop(lexer));
        token_free(tok);
        tok = lexer_pop(lexer);
        //could get exit $var ??
        if (tok->word != NULL && isdigit(tok->word[0]))
        {
            child->fd = atoi(tok->word);
        }
        else
            child->fd = 0;
        //might need to pop somewhere here
        (*ast)->children = realloc((*ast)->children, ((*ast)->nb_children + 1) * sizeof(struct ast *));
        (*ast)->children[(*ast)->nb_children] = child;
        (*ast)->nb_children += 1;
    }
    else if (tok->type == UNSET)
    {
        child = ast_new(AST_UNSET);
        token_free(lexer_pop(lexer));
        token_free(tok);
        tok = lexer_pop(lexer);
        //we might need a radical change
        if (tok->word != NULL && (!strcmp(tok->word, "-n") || !strcmp(tok->word, "-f")))
        {
            child->cmd.cmd = strdup(tok->word);
            token_free(tok);
            tok = lexer_peek(lexer);
        }
        if (!child->cmd.args)
            child->cmd.args = strdup(tok->word);
        else
        {
            child->cmd.args = realloc(child->cmd.args, \
                sizeof(char) * (strlen(child->cmd.args) + strlen(tok->word)) + 2);
            strcat(child->cmd.args, " ");
            strcat(child->cmd.args, tok->word);
        }  
        //might need to pop somewhere here
        (*ast)->children = realloc((*ast)->children, ((*ast)->nb_children + 1) * sizeof(struct ast *));
        (*ast)->children[(*ast)->nb_children] = child;
        (*ast)->nb_children += 1;
        token_free(tok);
        tok = lexer_pop(lexer);
    }
    else if (tok->type == ECHOO)
    {
        //if there are some exported fucntions then we do not expand variable here
        //fix tmrw!!
        int went = 0;
        child = ast_new(AST_CMD);
        child->cmd.cmd = strdup("echo");
        token_free(tok);
        token_free(lexer_pop(lexer));
        tok = lexer_pop(lexer);
        if (tok->word != NULL && (!strcmp(tok->word, "-n") || !strcmp(tok->word, "-e")))
        {
            child->cmd.args = strdup(tok->word);
            token_free(tok);
            //token_free(lexer_pop(lexer));
            tok = lexer_peek(lexer);
            went = 1;
        }
        if (!child->cmd.args)
            child->cmd.args = strdup(tok->word);
        else
        {
            child->cmd.args = realloc(child->cmd.args, \
                sizeof(char) * (strlen(child->cmd.args) + strlen(tok->word)) + 2);
            strcat(child->cmd.args, " ");
            strcat(child->cmd.args, tok->word);
        }  
        //might need to pop somewhere here
        (*ast)->children = realloc((*ast)->children, ((*ast)->nb_children + 1) * sizeof(struct ast *));
        (*ast)->children[(*ast)->nb_children] = child;
        (*ast)->nb_children += 1;
        token_free(tok);
        if (went)
            tok = lexer_pop(lexer);
        else
            tok = lexer_peek(lexer);
    }
    else if (tok->type == DOT)
    {
        child = ast_new(AST_DOT);
        token_free(tok);
        token_free(lexer_pop(lexer));
        tok = lexer_peek(lexer);
        child->cmd.cmd = strdup(tok->word);
        token_free(lexer_pop(lexer));
        (*ast)->children = realloc((*ast)->children, ((*ast)->nb_children + 1) * sizeof(struct ast *));
        (*ast)->children[(*ast)->nb_children] = child;
        (*ast)->nb_children += 1;
    }
    else
    {
        child = ast_new(AST_NORED);
        struct ast *ast_cmd = ast_new(AST_CMD);
        child->children[0] = ast_cmd;
        child->nb_children += 1;
        enum parser_status status = parse_simple_cmd(&child, lexer);
        if (status != PARSER_OK)
        {
            token_free(tok);
            ast_free(child);
            return status;
        }
        (*ast)->children = realloc((*ast)->children, ((*ast)->nb_children + 1) * sizeof(struct ast *));
        (*ast)->children[(*ast)->nb_children] = child;
        (*ast)->nb_children += 1;
    }

    token_free(tok);
    return PARSER_OK;
}


static enum parser_status parse_simple_cmd(struct ast **child, struct lexer *lexer)
{
    int no_pre = 0;
    enum parser_status status = parse_prefix(child, lexer);
    if (status != PARSER_OK)
        no_pre = 1;
    while (!no_pre)
    {
        status = parse_prefix(child, lexer);
        if (status != PARSER_OK)
            break;
    } 
    status = parse_element(child, lexer);
    if (status != PARSER_OK && no_pre)
        return PARSER_UNEXPECTED_TOKEN;
    while (true)
    {
        status = parse_element(child, lexer);
        if (status != PARSER_OK)
            break;
    }

    (*child)->children[0]->cmd.cmd_arr = realloc((*child)->children[0]->cmd.cmd_arr, ((*child)->children[0]->cmd.index + 1 ) * sizeof(char *));
    (*child)->children[0]->cmd.cmd_arr[(*child)->children[0]->cmd.index] = NULL; 

    return PARSER_OK;
}

static enum parser_status parse_prefix(struct ast **child, struct lexer *lexer)
{
    struct token *tok = lexer_peek(lexer);
    if (tok->type == VAR_ASSIGN)
    {
        (*child)->type = AST_VAR;
        (*child)->cmd.type = VAR;
        (*child)->cmd.cmd = strdup(tok->word);
        token_free(tok);
        token_free(lexer_pop(lexer));
        return PARSER_OK;
    }
    enum parser_status status = parse_redirection(child, lexer);
    if (status != PARSER_OK)
    {
        token_free(tok);
        return status;
    }
    token_free(tok);
    return PARSER_OK;
}

static enum parser_status parse_element(struct ast **child, struct lexer *lexer)
{
    struct token *tok = lexer_peek(lexer);
    struct command *cmd = &((*child)->children[0]->cmd);
    if (tok->type == WORD)
    {
        if (cmd->empty)
        {
            ((*child)->children[0]->cmd).type = get_cmd_type(tok->word);
            cmd->cmd = calloc(strlen(tok->word) + 1, sizeof(char));
            cmd->cmd = strcat(cmd->cmd, tok->word);
            cmd->cmd_arr = malloc((cmd->index + 1 ) * sizeof(char *));
            cmd->cmd_arr[cmd->index++] = strdup(tok->word);
            cmd->empty = 0;
        }
        else
        {
            size_t len = (cmd->args == NULL) ? 0 : strlen(cmd->args);
            cmd->args = realloc(cmd->args, len + strlen(tok->word) + 2);

            if (len == 0)
                cmd->args[len] = '\0';
            else
                cmd->args = strcat(cmd->args, " ");

            cmd->args = strcat(cmd->args, tok->word);

            cmd->cmd = realloc(cmd->cmd, strlen(cmd->cmd) + strlen(tok->word) + 2);
            cmd->cmd = strcat(cmd->cmd, " ");
            cmd->cmd = strcat(cmd->cmd, tok->word);
            cmd->cmd_arr = realloc(cmd->cmd_arr, (cmd->index + 1 ) * sizeof(char *));
            cmd->cmd_arr[cmd->index++] = strdup(tok->word);
        }
        token_free(tok);
        token_free(lexer_pop(lexer));
        return PARSER_OK;
    }
    token_free(tok);

    enum parser_status status = parse_redirection(child, lexer);
    if (status != PARSER_OK)
        return status;

    return PARSER_OK;
}

static enum parser_status parse_redirection(struct ast **child, struct lexer *lexer)
{
    struct token *tok = lexer_peek(lexer);
    struct ast *ast_ion = ast_new(AST_ION);
    if (tok->type == ION)
    {
        ast_ion->fd = atoi(tok->word);
        token_free(lexer_pop(lexer));
    }
    token_free(tok);
    tok = lexer_peek(lexer);

    struct ast *ast_red = NULL;
    if (tok->type == RED_L)
    {
        ast_red = ast_new(AST_REDL);
        token_free(lexer_pop(lexer));
        ast_red->cmd.cmd = strdup(tok->word);
    }

    else if (tok->type == RED_R)
    {
        ast_red = ast_new(AST_REDR);
        token_free(lexer_pop(lexer));
        ast_red->cmd.cmd = strdup(tok->word);
    }

    else
    {
        token_free(tok);
        ast_free(ast_ion);
        return PARSER_UNEXPECTED_TOKEN;
    }

    token_free(tok);
    tok = lexer_peek(lexer);

    if (tok->type != WORD)
    {
        token_free(tok);
        ast_free(ast_ion);
        return PARSER_UNEXPECTED_TOKEN;
    }
    struct ast *ast_file = ast_new(AST_FILE);
    ast_file->cmd.type = FD;
    ast_file->cmd.cmd = strdup(tok->word);
    ast_file->cmd.empty = 0;

    ast_red->children = realloc(ast_red->children, 2 * sizeof(struct ast *));
    ast_red->children[0] = ast_ion;
    ast_red->children[1] = ast_file;
    (ast_red->nb_children) += 2;

    (*child)->type = AST_RED;
    (*child)->children = realloc((*child)->children, ((*child)->nb_children + 1) * sizeof(struct ast *));
    (*child)->children[(*child)->nb_children] = ast_red;
    (*child)->nb_children += 1;

    return PARSER_OK;
}

static enum parser_status parse_shell_cmd(struct ast **ast, struct lexer *lexer)
{
    struct token *tok = lexer_peek(lexer);
    enum parser_status status = 0;
    struct ast *child = NULL;
    if (tok->type == IF)
    {
        child = ast_new(AST_IF);
        status = parse_rule_if(&child, lexer);
        if (status != PARSER_OK)
        {
            (*ast)->children = realloc((*ast)->children, ((*ast)->nb_children + 1) * sizeof(struct ast *));
            (*ast)->children[(*ast)->nb_children] = child;
            (*ast)->nb_children += 1;
            token_free(tok);
            return status;
        }
    }
    else if (tok->type == WHILE)
    {
        child = ast_new(AST_WHILE);
        status = parse_rule_while(&child, lexer);
        if (status != PARSER_OK)
        {
            (*ast)->children = realloc((*ast)->children, ((*ast)->nb_children + 1) * sizeof(struct ast *));
            (*ast)->children[(*ast)->nb_children] = child;
            (*ast)->nb_children += 1;
            token_free(tok);
            return status;;
        }
    }
    else if (tok->type == FOR)
    {
        child = ast_new(AST_FOR);
        status = parse_rule_for(&child, lexer);
        if (status != PARSER_OK)
        {
            (*ast)->children = realloc((*ast)->children, ((*ast)->nb_children + 1) * sizeof(struct ast *));
            (*ast)->children[(*ast)->nb_children] = child;
            (*ast)->nb_children += 1;
            token_free(tok);
            return status;
        }
    }
    else if (tok->type == UNTIL)
    {
        child = ast_new(AST_UNTIL);
        status = parse_rule_until(&child, lexer);
        if (status != PARSER_OK)
        {
            (*ast)->children = realloc((*ast)->children, ((*ast)->nb_children + 1) * sizeof(struct ast *));
            (*ast)->children[(*ast)->nb_children] = child;
            (*ast)->nb_children += 1;
            token_free(tok);
            return status;
        }
    }
    else if (tok->type == CURL_L)
    {
        token_free(lexer_pop(lexer));
        child = ast_new(AST_BLOCK);
        status = parse_compound_list(&child, lexer);
        token_free(tok);
        if (status != PARSER_OK)
            return status;
        tok = lexer_peek(lexer);
        if (tok->type != CURL_R)
        {
            token_free(tok);
            return PARSER_UNEXPECTED_TOKEN;
        }
        token_free(lexer_pop(lexer));
    }
    
    else if (tok->type == BRACK_L)
    {
        token_free(lexer_pop(lexer));
        child = ast_new(AST_BLOCK);
        status = parse_compound_list(&child, lexer);
        token_free(tok);
        if (status != PARSER_OK)
            return status;
        tok = lexer_peek(lexer);
        if (tok->type != BRACK_R)
        {
            token_free(tok);
            return PARSER_UNEXPECTED_TOKEN;
        }
        token_free(lexer_pop(lexer));
    }
    (*ast)->children = realloc((*ast)->children, ((*ast)->nb_children + 1) * sizeof(struct ast *));
    (*ast)->children[(*ast)->nb_children] = child;
    (*ast)->nb_children += 1;

    token_free(tok);

    return PARSER_OK;

}

static enum parser_status parse_rule_if(struct ast **child, struct lexer *lexer)
{
    struct token *tok = lexer_peek(lexer);
    if (tok->type != IF)
    {
        token_free(tok);
        return PARSER_UNEXPECTED_TOKEN;
    }
    token_free(lexer_pop(lexer));

    //creat conditions child and parse it by calling compound list
    struct ast *cond = ast_new(AST_COND);
    enum parser_status status = parse_compound_list(&cond, lexer);
    if (status != PARSER_OK)
        return status;
    //add the cond child to the tree
    (*child)->children[(*child)->nb_children] = cond;
    (*child)->nb_children += 1;

    //check if then is there
    token_free(tok);
    tok = lexer_peek(lexer);
    if (tok->type != THEN)
    {
        token_free(tok);
        return PARSER_UNEXPECTED_TOKEN;
    }

    token_free(tok);
    token_free(lexer_pop(lexer));

    //creat then expr child and parse it by calling compound list
    struct ast *then = ast_new(AST_THEN);
    status = parse_compound_list(&then, lexer);
    if (status != PARSER_OK)
        return status;
    //add the then child to the tree
    (*child)->children = realloc((*child)->children, ((*child)->nb_children + 1) * sizeof(struct ast *));
    (*child)->children[(*child)->nb_children] = then;
    (*child)->nb_children += 1;

    //check if it is the end of the if
    tok = lexer_peek(lexer);
    if (tok->type == FI)
    {
        //add a FI child (only when there is no else)
        (*child)->children = realloc((*child)->children, ((*child)->nb_children + 1) * sizeof(struct ast *));
        (*child)->children[(*child)->nb_children] = ast_new(AST_FI);
        (*child)->nb_children += 1;
        token_free(tok);
        token_free(lexer_pop(lexer));
        return PARSER_OK;
    }

    //parse else clause (the function add the children to the tree)
    token_free(tok);
    status = parse_else_clause(child, lexer);
    if (status != PARSER_OK)
    {
        return status;
    }
    tok = lexer_peek(lexer);
    if (tok->type != FI)
    {
        token_free(tok);
        return PARSER_UNEXPECTED_TOKEN;
    }
    token_free(tok);
    token_free(lexer_pop(lexer));
    return PARSER_OK;
}

static enum parser_status parse_compound_list(struct ast **ast, struct lexer *lexer)
{
    struct token *tok = lexer_peek(lexer);
    while (tok->type == BACK_SN)
    {
        token_free(lexer_pop(lexer));
        token_free(tok);
        tok = lexer_peek(lexer);
    }


    enum parser_status status = parse_and_or(ast, lexer);
    if (status != PARSER_OK)
    {
        token_free(tok);
        return status;
    }


    while (true)
    {
        token_free(tok);

        tok = lexer_peek(lexer);
        if ((tok->type != SEMICOLON) && tok->type != BACK_SN)
            break;

        token_free(tok);
        token_free(lexer_pop(lexer));
        tok = lexer_peek(lexer);
        while (tok->type == BACK_SN)
        {
            token_free(lexer_pop(lexer));
            token_free(tok);
            tok = lexer_peek(lexer);
        }


        status = parse_and_or(ast, lexer);
        if (status != PARSER_OK)
        {
            break;
        }
        //we should make an ast node for the and_or word
        //then we we call compoud list  again, this time with a mode
    }

    token_free(tok);
    return PARSER_OK;
}

static enum parser_status parse_and_or(struct ast **ast, struct lexer *lexer)
{
    enum parser_status status = parse_pipeline(ast, lexer);
    if (status != PARSER_OK)
        return status;
    struct token *tok = NULL;

    while (true)
    {
        tok = lexer_peek(lexer);
        if (tok->type != AND && tok->type != OR)
        {
            token_free(tok);
            break;
        }

        struct ast *ast_and_or = (tok->type == AND) ? ast_new(AST_AND) : ast_new(AST_OR);

        token_free(tok);
        token_free(lexer_pop(lexer));
        tok = lexer_peek(lexer);
        while (tok->type == BACK_SN)
        {
            token_free(tok);
            token_free(lexer_pop(lexer));
            tok = lexer_peek(lexer);
        }

        status = parse_pipeline(ast, lexer);
        if (status != PARSER_OK)
        {
            token_free(tok);
            return status;
        }

        (*ast)->children = realloc((*ast)->children, ((*ast)->nb_children + 1) * sizeof(struct ast *));
        (*ast)->children[(*ast)->nb_children] = ast_and_or;
        (*ast)->nb_children += 1;
        token_free(tok);
    }

    return PARSER_OK;
}


static enum parser_status parse_pipeline(struct ast **ast, struct lexer *lexer)
{
    //here, we make an ast that will hold the comand we first see.
    //can we pipe TRUE / FALSE to something?
    //here, we should check how to create a node to pipe one child to the other

    struct token *tok = NULL;
    //HERE IF WE HAVE NEGATION, IN EVAL, JUST REVERSE THE FUCKING
    // RETUNRNRNRNR VALUE :)))
    tok = lexer_peek(lexer);
    if (tok->type == NEGATION)
    {
        token_free(lexer_pop(lexer));
        (*ast)->negation = 1;
    }

    token_free(tok);

    struct ast *ast_pipe = ast_new(AST_NOPIPE);
    enum parser_status status = parse_cmd(&ast_pipe, lexer);
    if (status != PARSER_OK)
    {
        ast_free(ast_pipe);
        return status;
    }

    while (true)
    {
        tok = lexer_peek(lexer);
        if (tok->type != PIPE)
        {
            token_free(tok);
            break;
        }
        ast_pipe->type = AST_PIPE;

        token_free(tok);
        token_free(lexer_pop(lexer));
        tok = lexer_peek(lexer);
        while (tok->type == BACK_SN)
        {
            token_free(tok);
            token_free(lexer_pop(lexer));
            tok = lexer_peek(lexer);
        }

        status = parse_cmd(&ast_pipe, lexer);
        if (status != PARSER_OK)
        {
            ast_free(ast_pipe);
            token_free(tok);
            return status;
        }
        token_free(tok);
    }

    (*ast)->children = realloc((*ast)->children, ((*ast)->nb_children + 1) * sizeof(struct ast *));
    (*ast)->children[(*ast)->nb_children] = ast_pipe;
    (*ast)->nb_children += 1;

    return PARSER_OK;
}


static enum parser_status parse_else_clause(struct ast **child, struct lexer *lexer)
{
    enum parser_status status = 0;

    struct token *tok = lexer_peek(lexer);
    if (tok->type == ELSE)
    {
        token_free(lexer_pop(lexer));
        //for else: create new child parse the compound and add it to the tree
        struct ast *else_child = ast_new(AST_ELSE);
        status = parse_compound_list(&else_child, lexer);
        if (status != PARSER_OK)
            return status;

        (*child)->children = realloc((*child)->children, ((*child)->nb_children + 1) * sizeof(struct ast *));
        (*child)->children[(*child)->nb_children] = else_child;
        (*child)->nb_children += 1;
        token_free(tok);
        return status;
    }
    else if (tok->type == ELIF)
    {
        token_free(lexer_pop(lexer));

        //for elif: create new child of type if parse the cond, then and else clause
        struct ast *elif_child = ast_new(AST_IF);


        struct ast *cond = ast_new(AST_COND);
        status = parse_compound_list(&cond, lexer);
        if (status != PARSER_OK)
            return status;
        //add the cond child to the tree
        elif_child->children = realloc(elif_child->children, (elif_child->nb_children + 1) * sizeof(struct ast *));
        elif_child->children[elif_child->nb_children] = cond;
        elif_child->nb_children += 1;


        token_free(tok);
        tok = lexer_pop(lexer);
        if (tok->type != THEN)
            return PARSER_UNEXPECTED_TOKEN;

        struct ast *then = ast_new(AST_THEN);
        status = parse_compound_list(&then, lexer);
        if (status != PARSER_OK)
            return status;
        //add the cond child to the tree
        elif_child->children = realloc(elif_child->children, (elif_child->nb_children + 1) * sizeof(struct ast *));
        elif_child->children[elif_child->nb_children] = then;
        elif_child->nb_children += 1;

        status = parse_else_clause(&elif_child, lexer);
        if (status != PARSER_OK)
        {
            elif_child->children = realloc(elif_child->children, (elif_child->nb_children + 1) * sizeof(struct ast *));
            elif_child->children[elif_child->nb_children] = ast_new(AST_FI);
            elif_child->nb_children += 1;
        }

        (*child)->children = realloc((*child)->children, ((*child)->nb_children + 1) * sizeof(struct ast *));
        (*child)->children[(*child)->nb_children] = elif_child;
        (*child)->nb_children += 1;
        token_free(tok);
        return PARSER_OK;
    }
    token_free(tok);
    return PARSER_UNEXPECTED_TOKEN;
}

static enum parser_status parse_rule_while(struct ast **child, struct lexer *lexer)
{
    struct token *tok = lexer_peek(lexer);

    if (tok->type != WHILE)
    {
        token_free(tok);
        return PARSER_UNEXPECTED_TOKEN;
    }
    token_free(tok);
    token_free(lexer_pop(lexer));
    
    struct ast *cond = ast_new(AST_COND);
    enum parser_status status = parse_compound_list(&cond, lexer);
    if (status != PARSER_OK)
        return status;

    (*child)->children[(*child)->nb_children] = cond;
    (*child)->nb_children += 1;

    //dogroup AST MAKINGS

    struct ast *do_child = ast_new(AST_DO);
    status = parse_rule_do_group(&do_child, lexer);
    if (status != PARSER_OK)
    {
        return PARSER_UNEXPECTED_TOKEN;
    }

    (*child)->children = realloc((*child)->children, ((*child)->nb_children + 1) * sizeof(struct ast *));
    (*child)->children[(*child)->nb_children] = do_child;
    (*child)->nb_children += 1;
    return PARSER_OK;
}

static enum parser_status parse_rule_until(struct ast **child, struct lexer *lexer)
{
    struct token *tok = lexer_peek(lexer);

    if (tok->type != UNTIL)
    {
        token_free(tok);
        return PARSER_UNEXPECTED_TOKEN;
    }

    token_free(tok);
    tok = lexer_pop(lexer);

    struct ast *cond = ast_new(AST_COND);
    enum parser_status status = parse_compound_list(&cond, lexer);
    if (status != PARSER_OK)
        return status;

    (*child)->children[(*child)->nb_children] = cond;
    (*child)->nb_children += 1;

    //dogroup AST MAKINGS

    struct ast *do_child = ast_new(AST_DO);
    status = parse_rule_do_group(&do_child, lexer);
    if (status != PARSER_OK)
    {
        token_free(tok);
        return PARSER_UNEXPECTED_TOKEN;
    }

    (*child)->children = realloc((*child)->children, ((*child)->nb_children + 1) * sizeof(struct ast *));
    (*child)->children[(*child)->nb_children] = do_child;
    (*child)->nb_children += 1;
    token_free(tok);
    return PARSER_OK;
}

static enum parser_status parse_rule_do_group(struct ast **child, struct lexer *lexer)
{
    struct token *tok = lexer_peek(lexer);

    if (tok->type != DO)
    {
        token_free(tok);
        return PARSER_UNEXPECTED_TOKEN;
    }
    token_free(tok);
    token_free(lexer_pop(lexer));

   // struct ast *cmd = ast_new(AST_CMD);
    enum parser_status status = parse_compound_list(child, lexer);
    if (status != PARSER_OK)
        return status;

    tok = lexer_pop(lexer);
    if (tok->type != DONE)
    {
        token_free(tok);
        return PARSER_UNEXPECTED_TOKEN;
    }
    token_free(tok);
    return PARSER_OK;
}

static enum parser_status parse_rule_for(struct ast **child, struct lexer *lexer)
{
    struct token *tok = lexer_peek(lexer);
    enum parser_status status = 0;
    if (tok->type != FOR)
    {
        token_free(tok);
        return PARSER_UNEXPECTED_TOKEN;
    }
    
    token_free(tok);
    token_free(lexer_pop(lexer));
    tok = lexer_peek(lexer);

    if (tok->type != WORD)
    {   
        token_free(tok);
        return PARSER_UNEXPECTED_TOKEN;
    }

    struct ast *cond = ast_new(AST_COND);

    struct ast *var_child = ast_new(AST_VAR);
    var_child->cmd.cmd = strdup(tok->word);

    (cond)->children = realloc((cond)->children, 
            ((cond)->nb_children + 1) * sizeof(struct ast *));
    (cond)->children[(cond)->nb_children] = var_child;
    (cond)->nb_children += 1;

    (*child)->children = realloc((*child)->children, 
            ((*child)->nb_children + 1) * sizeof(struct ast *));
    (*child)->children[(*child)->nb_children] = cond;
    (*child)->nb_children += 1;

    token_free(tok);
    token_free(lexer_pop(lexer));
    tok = lexer_peek(lexer);
    if (tok->type == SEMICOLON)
    {
        token_free(lexer_pop(lexer));
    }
    else if (tok->type == BACK_SN)
    {
        while (tok->type == BACK_SN)
        {
            token_free(tok);
            token_free(lexer_pop(lexer));
            tok = lexer_peek(lexer);
        }
    }
    token_free(tok);

    tok = lexer_peek(lexer);
    if (tok->type == IN)
    {
        token_free(tok);
        token_free(lexer_pop(lexer));
        while (true)
        {
            tok = lexer_peek(lexer);
            if (tok->type != WORD)
            {
                token_free(tok);
                break;
            }


            struct ast *in_child = ast_new(AST_VAR);
            in_child->cmd.cmd = strdup(tok->word);

            (cond)->children = realloc((cond)->children, 
                    ((cond)->nb_children + 1) * sizeof(struct ast *));
            (cond)->children[(cond)->nb_children] = in_child;
            (cond)->nb_children += 1;

            token_free(tok);
            token_free(lexer_pop(lexer));
        }
        tok = lexer_peek(lexer);
        if (tok->type != SEMICOLON && tok->type != BACK_SN)
        {
            token_free(tok);
            return PARSER_UNEXPECTED_TOKEN;
        }
        token_free(tok);
        token_free(lexer_pop(lexer));
    }
    else
        token_free(tok);

    struct ast *do_child = ast_new(AST_DO);
    status = parse_rule_do_group(&do_child, lexer);
    if (status != PARSER_OK)
    {
        //token_free(tok);
        return PARSER_UNEXPECTED_TOKEN;
    }

    (*child)->children = realloc((*child)->children, 
            ((*child)->nb_children + 1) * sizeof(struct ast *));
    (*child)->children[(*child)->nb_children] = do_child;
    (*child)->nb_children += 1;
    return PARSER_OK;
}
