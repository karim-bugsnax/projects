#include <err.h>
#include <io/cstream.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <utils/vec.h>

#include "parser/parser.h"
#include "eval_ast/env.h"
#include "eval_ast/eval_ast.h"
#include "lexer/lexer.h"
#include "token/token.h"
#include "ast/ast.h"
#include "func_list/func.h"


int main(int argc, char *argv[])
{
    struct lexer *lex = NULL;
    struct ast *mom = NULL;
    struct env *env = init_env(argc, argv);
    struct func *func_list = NULL;
    int res = 0;
    int oneline = 0;
    enum parser_status status = PARSER_CONTINUE;
    if (argc == 1)
    {
        char buf_stdin[6969];
        fread(buf_stdin, sizeof(char), 6969, stdin);
        lex = lexer_new(buf_stdin);
        mom = ast_new(AST_MOM);
        status = parse_input(&mom, lex, &func_list);
        res = eval_tree(&mom, &env); 
        lexer_free(lex);
        ast_free(mom);
        env_free(env);
    }
    else if (strcmp(argv[1], "-c") == 0)
    {
        lex = lexer_new(argv[2]);
        mom = ast_new(AST_MOM);
        status = parse_input(&mom, lex, &func_list);
        res = eval_tree(&mom, &env); 
        lexer_free(lex);
        ast_free(mom);
        env_free(env);
    }
    else if (strcmp(argv[1], "<") == 0)
    {
        char buf[6969];

        FILE *fd;
        if (argc == 2)
            fd = fopen(argv[1], "r");
        else
            fd = fopen(argv[2], "r");
        if (fd == NULL)
            errx(2, "file not found");
        fread(buf, sizeof(char), 6969, fd);
        lex = lexer_new(buf);
        mom = ast_new(AST_MOM); //doin' ur mom
        status = parse_input(&mom, lex, &func_list);
        while (status == PARSER_CONTINUE)
        {
            oneline = 1;
            status = parse_input(&mom, lex, &func_list);
        }
        if (oneline && status != PARSER_UNEXPECTED_TOKEN)
            res = eval_tree(&mom, &env); 
        lexer_free(lex);
        ast_free(mom);
        env_free(env);
    }
    else
    {
        char buf[6969];

        FILE *fd;
        fd = fopen(argv[1], "r");
        if (fd == NULL)
            errx(2, "file not found");
        fread(buf, sizeof(char), 6969, fd);
        lex = lexer_new(buf);
        mom = ast_new(AST_MOM); //doin' ur mom
        status = PARSER_CONTINUE;
        while (status == PARSER_CONTINUE)
        {
            oneline = 1;
            status = parse_input(&mom, lex, &func_list);
            if (mom->fd == 69)
                break;
            token_free(lexer_pop(lex));
        }
        if (oneline && status != PARSER_UNEXPECTED_TOKEN)
            res = eval_tree(&mom, &env); 
        ast_free(mom);
        lexer_free(lex);
        env_free(env);
    }
    if (status == 1)
        errx(2, "unexpected token");
    if (res == -1)
        return 0;
    return res;
}