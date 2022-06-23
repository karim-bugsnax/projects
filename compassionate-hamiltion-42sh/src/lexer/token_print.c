#include <stdio.h>

#include "lexer.h"
#include "../eval_ast/env.h"
#include "../eval_ast/variable_expansion.h"
#include "../token/token.h"

char *tab[] = {
    [IF] = "IF", [ELSE] = "ELSE", [ELIF] = "ELIF", [THEN] = "THEN",
    [FI] = "FI", [SEMICOLON] = "SEMICOLON", [BACK_SN] = "BACK_SN", [WORD] = "WORD",
    [ERROR] = "ERROR", [TOK_EOF] = "EOF", [AND] = "AND", [OR] = "OR",
    [PIPE] = "PIPE", [RED_R] = "RED_R", [RED_L] = "RED_L", [WHILE] = "WHILE",
    [UNTIL] = "UNTIL", [FOR] = "FOR", [DO] = "DO", [DONE] = "DONE", [ION] = "ION",
    [IN] = "IN", [NEGATION] = "NEGATION", [VAR_ASSIGN] = "VAR_ASSIGN",
    [BRACK_L] = "BRACK_L", [BRACK_R] = "BRACK_R", [UNSET] = "UNSET", [EXPORT] = "EXPORT",
    [CD] = "CD", [EXIT] = "EXIT", [CURL_L] = "CURL_L", [CURL_R] = "CURL_R"
};

void token_print(char *in)
{
    printf("IN:\n%s\n\n", in);

    struct lexer *lexer = lexer_new(in);
    struct token *token = lexer_pop(lexer);

    char *args[] = {"program", "echo", NULL};
    struct env *env = init_env(2, args);

    env = env_push_direct(env, "test", "100");

    while (token->type != TOK_EOF && token->type != ERROR)
    {
        if (token->type == WORD)
        {
            printf("%s %s\n", tab[token->type], token->word);
            //token->word = variableExpansion(env, token->word);
            //printf("->%s\n", token->word);
        }
        else if (token->type == VAR_ASSIGN)
            printf("%s %s\n", tab[token->type], token->word);
        else
            printf("%s\n", tab[token->type]);

        if (token->type == VAR_ASSIGN)
            env = env_push_word(env, token->word);

        token_free(token);
        token = lexer_pop(lexer);
    }

    printf("%s\n", tab[token->type]);

    token_free(token);
    lexer_free(lexer);
    env_free(env);
}

/*
#include <string.h>
#include <stdlib.h>
int main(void)
{
    //char *in = "if true; then echo 123; elif true and false; else echo '456 echo 13245'; fi #IGNORE THIS; SHITE\n if true or false; then echo Winnieh; else poppo | foo";
    //char *in = "if if echo hi; then echo yes; fi then echo 1st ; fi"; // if not word
    //char *in = "if echo if; then echo then\n; fi"; // second if word
    //char *in = "var=1 shit \"123\" '$(var)' \\\"$(var) $e"; // test scaping
    //char *in = "if true&&false; then if true||false; then echo afs | tr a d; fi"; // testing && || as delimiters

    //char *in = "0\"1$var1\\$${var1}\"{$var1\"}\"\\\"~Hello\\\""; // test var expansion advance
    // output: 011$1{1}"~Hello"

    //char *in = "| > < >& <& >> <> >| 1> 2> echo afs|tr a e";
    //char *in = "| if true||false&&true || false && true";
    //char *in = "file1 2> err.txt 1> out.txt > lol > < >& <& >> <> >|";

    //char *in = "2> 2<& var=1; echo ${var} $var; $* $1;";// $var$var 2\"$var\"2";

    //char *in = "echo \2>a"; // writes 2 into a
    //char *in = "echo 2\>a"; // writes 2>a to stdout

    //char *in = "DATE=`date`\n\"echo $DATE\"";

    //char *in = "{}; var=var; echo ${var}; { echo a; echo b; } ; { echo a; echo b;};{ echo a; echo b; }; {}";
    
    //char *in = "env=\"1 2 3\"\nfor for in \"$env\"; do\n\techo $var\ndone;";
    
    char *in = "echo word); $(var); echo ${var}; echo {test}; fun() { echo a }; if (true&&false)";
    token_print(in);

    struct env *env = NULL;
    env = env_push_direct(env, "var", "AyA");
    char *string = malloc(sizeof(char) * 30);
    strcpy(string, "var=\"1 2 3\"");
    env = env_push_word(env, string);
    printf("%s %s\n", env->var_name, env->var_value);
    env_free(env);
    free(string);
    struct env *env = NULL;
    env = env_push_direct(env, "env", "1 2 3");

    char *s = malloc(30);
    strcpy(s, "\"$env\"");

    s = escape_ma_quotes(s);
    printf("%s\n", s);

    s = variableExpansion(env, s);
    printf("%s\n", s);


    free(s);
    env_free(env);

    return 0;
}
*/