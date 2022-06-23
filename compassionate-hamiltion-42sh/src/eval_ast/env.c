#define _XOPEN_SOURCE 500

#include "env.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#include "variable_expansion_expansion.h"
#include "../lexer/lexer_helper.h"
#include "../token/token.h"

struct env *env_push_word(struct env *env, char *var_assign)
{
    int start = 0;
    int end;
    for (end = 0; var_assign[end] != '='; end++);
    char *var_name = cutOutSegment(var_assign, start, end - 1);

    start = end + 1;
    for (end = start; var_assign[end] != '\0'; end++);

    if (var_assign[start] == '"' || var_assign[start] == '\'')
    {
        start += 1;
        end -= 1;
    }
    char *var_value = cutOutSegment(var_assign, start, end - 1);

    struct env *element = malloc(sizeof(struct env));
    element->var_name = var_name;
    element->var_value = var_value;
    element->next = env;
    element->is_export = false;

    return element;
}

// BEHAVIIOR: pushes an env element in the front. ("var", "1") version
// FUNCT: handles redefinition of variables
struct env *env_push_direct(struct env *env, char *var_name, char *var_value)
{
    struct env *element = malloc(sizeof(struct env));
    element->var_name = strdup(var_name);
    element->var_value = strdup(var_value);
    element->next = env;
    element->is_export = false;

    return element;
}

// BEHAVIOR: search for the env with the given var_name
struct env *env_search(struct env *env, char *var_name)
{
    while (env != NULL)
    {
        if (strcmp(env->var_name, var_name) == 0)
            break;
        env = env->next;
    }

    return env;
}

// BEHAVIOR: free the env linked list
void env_free(struct env *env)
{
    while (env != NULL)
    {
        struct env *env_pointer = env->next;
        free(env->var_name);
        free(env->var_value);
        free(env);
        env = env_pointer;
    }
}

// Below are all functions for initializing the env (pushing global variables)
char *my_itoa(int value)
{
    char *s = malloc(sizeof(char) * 20);
    int adjustment = 0;

    if (value == 0)
    {
        *(s + 0) = '0';
        adjustment = 1;
    }

    char reversed_s[15]; // will be big enough, even for INT_MAX
    int j = 0;
    while (value > 0)
    {
        int tmp = value % 10;
        value /= 10;
        *(reversed_s + j) = tmp + '0';
        j++;
    }
    reversed_s[j] = '\0';

    for (int tmp = 0; tmp < j; tmp++)
        *(s + tmp + adjustment) = *(reversed_s + (j - tmp - 1));

    *(s + j + adjustment) = '\0';
    return s;
}

char *concat_strings(int argc, char *argv[])
{
    if (argc == 1)
        return NULL;

    size_t len = 0;
    for(int i = 1; i < argc; i++)
        len += strlen(argv[i]) + 1;
    len += 1;

    char *res = calloc(sizeof(char), len);

    for(int i = 1; i < argc - 1; i++)
    {
        strcat(res, argv[i]);
        strcat(res, " ");
    }
    strcat(res, argv[argc - 1]);

    return res;
}

// Pushes all basic environmental variables (ordered by usage frequency)
struct env *init_env(int argc, char *argv[])
{
    struct env *env = NULL;
    char *ito = NULL;


    ito = my_itoa(argc);
    env = env_push_direct(env, "#", ito); // $#
    free(ito);

    ito = my_itoa(getuid());
    env = env_push_direct(env, "UID", ito); // $UID
    free(ito);

    env = env_push_direct(env, "IFS", " \t\n"); // $IFS
    ito = my_itoa(getpid());
    env = env_push_direct(env, "$", ito); // $$
    free(ito);

    char *merged_argv = concat_strings(argc, argv);
    if (merged_argv != NULL)
    {
        env = env_push_direct(env, "@", merged_argv); // $@
        env = env_push_direct(env, "*", merged_argv); // $*
        free(merged_argv);
    }

    for(int i = argc -1; i > 0; i--) //$1 ... $n
    {
        //you can have : ./42sh 42sh 42 sh
        //you cant assume they will be numbers
        ito = my_itoa(i);
        env = env_push_direct(env, ito, argv[i]);
        free(ito);
    }
    /* TODO
    $?; last return value
    $OLDPWD; PWD before the last cd command
    */

    return env;
}

struct env *env_get_export(struct env *env)
{
    struct env *export_env = NULL;

    while (env != NULL)
    {
        if (env->is_export == true)
        {
            struct env *element = malloc(sizeof(struct env));
            element->var_name = strdup(env->var_name);
            element->var_value = strdup(env->var_value);
            element->next = export_env;
            element->is_export = true;

            export_env = element;
        }
        env = env->next;
    }

    return export_env;
}