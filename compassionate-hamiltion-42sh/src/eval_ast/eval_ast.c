#include "../parser/parser.h"
#include "../ast/ast.h"
#include "../lexer/lexer.h"
#include "../lexer/lexer_helper.h"
#include "../func_list/func.h"
#include "env.h"
#include "pipe_redir.h"
#include "variable_expansion.h"
#include <err.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

static int eval_cmd(struct ast **cmd_node, struct env **env);
static int eval_if(struct ast **if_node, struct env **env);
static int eval_while(struct ast **while_node, struct env **env);
static int eval_until(struct ast **until_node, struct env **env);
static int eval_do_group(struct ast **do_group_node, struct env **env);
static int eval_var_assign(struct ast **var_node, struct env **env);
static int eval_for(struct ast **var_node, struct env **env);
static int eval_continue_break(struct ast **cont_node, struct env **env);
static int eval_cd(struct ast **cd_node, struct env **env);
static int eval_exit(struct ast **exit_node, struct env **env);
static int eval_export(struct ast **export_node, struct env **env);
static int eval_dot(struct ast **dot_node, struct env **env);
static int eval_or(struct ast **group_node, int index);
static int eval_and(struct ast **group_node, int index);
static int eval_unset(struct ast **unset_node,struct env **env);

static int eval_or(struct ast **group_node, int index)
{
    return ((*group_node)->children[index - 1]->cond | (*group_node)->children[index - 2]->cond);
}

static int eval_and(struct ast **group_node, int index)
{
    return ((*group_node)->children[index - 1]->cond & (*group_node)->children[index - 2]->cond);
}

typedef int (*const operation)(struct ast **, struct env **);
static const operation operations[] = {
    [AST_IF] = eval_if,
    [AST_CMD] = eval_cmd,
    [AST_WHILE] = eval_while,
    [AST_UNTIL] = eval_until,
    [AST_PIPE] = eval_pipe,
    [AST_REDIR] = eval_redirection,
    [AST_VAR] = eval_var_assign,
    [AST_EXPORT] = eval_export,
    [AST_NORED] = eval_do_group,
    [AST_NOPIPE] = eval_do_group,
    [AST_DOT] = eval_dot,
    [AST_FOR] = eval_for,
    [AST_BRK] = eval_continue_break,
    [AST_CONT] = eval_continue_break,
    [AST_CD] = eval_cd,
    [AST_EXIT] = eval_exit,
    [AST_BLOCK] = eval_do_group,
    [AST_AND] = eval_do_group,
    [AST_OR] = eval_do_group,
    [AST_UNSET] = eval_unset,
};


static char *escape_ma_quotes(char *s)
{
    int len = strlen(s);
    char *res = malloc(sizeof(char) * (len * 2));
    int res_i = 0;
    for (int i = 0; i < len; i++)
    {
        if ((i == 0 && s[i] == '\"') || (s[i] == '\"' && s[i - 1] != '\\'))
        {
            res[res_i] = '\\';
            res[res_i + 1] = '\"';
            res_i += 2;
        }
        else
        {
            res[res_i] = s[i];
            res_i += 1;
        }
    }

    free(s);
    res[res_i] = '\0';
    return res;
}

static int count_words(char *str)
{
    int counter = 1;
    size_t index = 0;
    size_t len = strlen(str);
    while (index < len)
    {
        if (str[index++] == ' ')
            counter++;
    }
    return counter;
}

static int get_index(char *str, int inde)
{
    int counter = 1;
    size_t index = 0;
    size_t len = strlen(str);
    while (index < len)
    {
        if (str[index++] == ' ')
        {
            break;
        }
        counter++;
    }
    return counter + inde;
}

static char *get_word(char *str)
{
    char *res = malloc(sizeof(char));
    size_t index = 0;
    size_t len = strlen(str);
    while (index < len)
    {
        if (str[index] == ' ')
        {
            break;
        }
        res[index] = str[index];
        index++;
        res = realloc(res, (index + 1) * sizeof(char));
    }
    res = realloc(res, (index + 1) * sizeof(char));
    res[index] = '\0';
    return res;
}

static int execute_cmd(struct ast **ast)
{
    // a command either errxes or succeeds
    // unless FALSE / TRUE
    int c_pid = fork();
    if (c_pid == -1)
    {
        errx(1, "Fork error\n");
    }

    if (c_pid == 0)
    {
        // should use execvp lmnop
        //  SPLIT ast->cmd->Cmd into the cmd
        // strtok puts the nullterminator

        // another note : alexa and grace finished the redirect,
        // should we add now, or next week (i vote next week LMAO)
        /*
        char *cmd_name = strdup(strtok((*ast)->cmd.cmd, " "));
        const char *cmd = strdup((*ast)->cmd.args);
        int status = execvp(cmd_name, cmd);
        if (status == -1)
        {
            errx(1, "execl failed\n");
        }*/
        int status = execlp("/bin/sh", "supershell", "-c", (*ast)->cmd.cmd, NULL);
        if (status == -1)
        {
            errx(1, "execl failed\n");
        }
    }
    else
    {
        int wstatus;
        int child = waitpid(c_pid, &wstatus, 0);
        if (WIFEXITED(wstatus) == 0)
        {
            errx(1, "child failed : execl failed\n");
        }

        if (child == -1)
        {
            errx(1, "wait pid failed\n");
        }
        //printf("process exit status: %d\n", WEXITSTATUS(wstatus));
        return 0;
    }
    return 0;
}

static int eval_if(struct ast **if_node, struct env **env)
{
    int res = -1;
    (*if_node)->cond = 1;

    res = eval_do_group(&(*if_node)->children[0], env);
    if (res != -1)
        return res;
    
    (*if_node)->cont = (*if_node)->children[0]->cont;  
    (*if_node)->brk = (*if_node)->children[0]->brk;
    if ((*if_node)->children[0]->cond)
    {
        size_t numb_then = (*if_node)->children[1]->nb_children;

        for (size_t i = 0; i < numb_then; i++)
        {
            res = operations[(*if_node)->children[1]->children[i]->type](&((*if_node)->children[1]->children[i]), env);
            if (res != -1)
                return res;
            (*if_node)->children[1]->cont = (*if_node)->children[1]->children[i]->cont;  
            (*if_node)->children[1]->brk = (*if_node)->children[1]->children[i]->brk; 


            (*if_node)->cont = (*if_node)->children[1]->cont;  
            (*if_node)->brk = (*if_node)->children[1]->brk;
            if ((*if_node)->cont || (*if_node)->brk)
            {
                break;
            }
        }

    }
    else if ((*if_node)->children[2]->type == AST_FI)
        return -1;
    else if ((*if_node)->children[2]->type == AST_ELSE)
    {
        size_t numb_else = (*if_node)->children[2]->nb_children;

        for (size_t i = 0; i < numb_else; i++)
        {
            res = operations[(*if_node)->children[2]->children[i]->type](&((*if_node)->children[2]->children[i]), env);
            if (res != -1)
                return res;
            (*if_node)->cont = (*if_node)->children[i]->cont;  
            (*if_node)->brk = (*if_node)->children[i]->brk;
            if ((*if_node)->cont || (*if_node)->brk)
            {
                break;
            }
        }
        return res;
    }
    else if ((*if_node)->children[2]->type == AST_IF)
    {
        return eval_if(&(*if_node)->children[2], env);
    }
    else
        errx(2, "gave birth within 'if' subtree something that is not ELIF/ELSE/FI");
    return -1;
}

static int eval_cmd(struct ast **cmd_node, struct env **env)
{
    env = env;
    char *cmd_copy = strdup((*cmd_node)->cmd.cmd);
    char *args_copy = NULL;
    if ((*cmd_node)->cmd.args != NULL)
        args_copy = strdup((*cmd_node)->cmd.args);
    (*cmd_node)->cond = 1;

    (*cmd_node)->cmd.cmd = variableExpansion(*env, (*cmd_node)->cmd.cmd);
    if (!strcmp((*cmd_node)->cmd.cmd, "true"))
    {
        free((*cmd_node)->cmd.cmd);
        (*cmd_node)->cmd.cmd = strdup(cmd_copy);
        if ((*cmd_node)->cmd.args != NULL)
        {
            free((*cmd_node)->cmd.args);
            (*cmd_node)->cmd.args = strdup(args_copy);
            free(args_copy);
        }
        else
            (*cmd_node)->cmd.args = NULL;

        free(cmd_copy);
        return -1;
    }

    if (!strcmp((*cmd_node)->cmd.cmd, "false"))
    {
        (*cmd_node)->cond = 0;
        free((*cmd_node)->cmd.cmd);
        (*cmd_node)->cmd.cmd = strdup(cmd_copy);
        if ((*cmd_node)->cmd.args != NULL)
        {
            free((*cmd_node)->cmd.args);
            (*cmd_node)->cmd.args = strdup(args_copy);
            free(args_copy);
        }
        else
            (*cmd_node)->cmd.args = NULL;

        free(cmd_copy);
        return -1;
    }
    if ((*cmd_node)->cmd.type == ECHO)
    {
        // realloccate then
        size_t l = 0;
        if ((*cmd_node)->cmd.args[0] == '-')
        {
            if ((*cmd_node)->cmd.args[1] == 'e')
            {
                l += 3; // skip the -e then the space
                size_t leen1 = strlen((*cmd_node)->cmd.args);
                if (l < leen1 && (*cmd_node)->cmd.args[l] == '"')
                {
                    l += 1;
                }
                if (l < leen1 && (*cmd_node)->cmd.args[leen1 - 1] == '"')
                {
                    (*cmd_node)->cmd.args[leen1 - 1] = '\0';
                }
                leen1 = strlen((*cmd_node)->cmd.args);
                int next = 0;
                int oncee =0;
                for (size_t ind = l; ind < leen1; ind++)
                {
                    if (next == 0 && (*cmd_node)->cmd.args[ind] == '\\')
                    {
                        if (oncee == 0)
                        {
                            oncee = 1;
                            putchar(92);
                        }
                        next = 1;
                    }
                    else
                    {
                        if (next == 1)
                        {
                            if ((*cmd_node)->cmd.args[ind] == '\\')
                            {
                                putchar(92);
                            }
                            else if ((*cmd_node)->cmd.args[ind] == 'n')
                                putchar('\n');
                            else if ((*cmd_node)->cmd.args[ind] == 't')
                                putchar('\t');
                            else
                                putchar((*cmd_node)->cmd.args[ind]);
                        }
                        else
                            putchar((*cmd_node)->cmd.args[ind]);
                        next = 0;
                        ind++;
                    }

                }
                printf("\n");
            }
            else if ((*cmd_node)->cmd.args[1] == 'n')
            {
                (*cmd_node)->cmd.args = variableExpansion(*env, (*cmd_node)->cmd.args);
                printf("%s", (*cmd_node)->cmd.args + 3);
            }
            else
            {
                (*cmd_node)->cmd.args = variableExpansion(*env, (*cmd_node)->cmd.args);
                puts((*cmd_node)->cmd.args);
            }
        }
        else
        {
            (*cmd_node)->cmd.args = variableExpansion(*env, (*cmd_node)->cmd.args);
            puts((*cmd_node)->cmd.args);
        }
    }
    else
    {
        (*cmd_node)->cmd.args = variableExpansion(*env, (*cmd_node)->cmd.args);
        execute_cmd(cmd_node);
    }
    
    free((*cmd_node)->cmd.cmd);
    (*cmd_node)->cmd.cmd = strdup(cmd_copy);
    free((*cmd_node)->cmd.args);
    if (args_copy)
        (*cmd_node)->cmd.args = strdup(args_copy);
    free(cmd_copy);
    if (args_copy)
        free(args_copy);
    return -1;
}

static int eval_do_group(struct ast **do_group_node, struct env **env)
{
    int res = -1;
    (*do_group_node)->brk = 0;
    (*do_group_node)->cont = 0;
    for (size_t i = 0; i < (*do_group_node)->nb_children; i++)
    {
        size_t look_ahead = i + 1;
        if (look_ahead < (*do_group_node)->nb_children &&
        (*do_group_node)->children[look_ahead]->type == AST_OR)
        {
            if ((*do_group_node)->cond)
                (*do_group_node)->children[look_ahead]->cond = 1;
            (*do_group_node)->cond = (*do_group_node)->children[i]->cond;
            if ((*do_group_node)->negation == 1)
            (*do_group_node)->cond = ((*do_group_node)->cond == 1) ? 0 : 1;

            (*do_group_node)->cont = (*do_group_node)->children[i]->cont;  
            (*do_group_node)->brk = (*do_group_node)->children[i]->brk;
            if ((*do_group_node)->cont || (*do_group_node)->brk)
            {
                break;
            }
            continue;
            //(*do_group_node)->children[i]->cond = eval_or(&(*do_group_node), i);
            //(*do_group_node)->cond = (*do_group_node)->children[i]->cond;
            //return res;
        } 
        if (look_ahead < (*do_group_node)->nb_children &&
        (*do_group_node)->children[look_ahead]->type == AST_AND)
        {
            if ((*do_group_node)->cond == 0)
            {
                (*do_group_node)->children[look_ahead]->cond = 0;
                //return res;
                continue;
            }
            res = eval_do_group(&(*do_group_node)->children[i], env);
            if (res != -1)
                return res;
            (*do_group_node)->cond = (*do_group_node)->children[i]->cond;
            if ((*do_group_node)->negation == 1)
            (*do_group_node)->cond = ((*do_group_node)->cond == 1) ? 0 : 1;

            (*do_group_node)->cont = (*do_group_node)->children[i]->cont;  
            (*do_group_node)->brk = (*do_group_node)->children[i]->brk;
            if ((*do_group_node)->cont || (*do_group_node)->brk)
            {
                break;
            }
            continue;
            //return res;
        }   
        if ((*do_group_node)->children[i]->type == AST_OR)
        {
            (*do_group_node)->children[i]->cond = eval_or(&(*do_group_node), i);
            (*do_group_node)->cond = (*do_group_node)->children[i]->cond;
            //return res;
        }
        else if ((*do_group_node)->children[i]->type == AST_AND)
        {
            (*do_group_node)->children[i]->cond = eval_and(&(*do_group_node), i);
            (*do_group_node)->cond = (*do_group_node)->children[i]->cond;

            //return res;
        }

        res = operations[(*do_group_node)->children[i]->type](&((*do_group_node)->children[i]), env);
        if (res != -1)
            return res;
        (*do_group_node)->cond = (*do_group_node)->children[i]->cond;

        if ((*do_group_node)->negation == 1)
            (*do_group_node)->cond = ((*do_group_node)->cond == 1) ? 0 : 1;

        (*do_group_node)->cont = (*do_group_node)->children[i]->cont;  
        (*do_group_node)->brk = (*do_group_node)->children[i]->brk;
        if ((*do_group_node)->cont || (*do_group_node)->brk)
        {
            break;
        }
    }
    return res;
}

static int eval_while(struct ast **while_node, struct env **env)
{
    int res = -1;
    (*while_node)->brk = 0;
    (*while_node)->cont = 0;
    do
    {
        res = eval_do_group(&(*while_node)->children[0], env);
        if (res != -1)
            return res;
        if ((*while_node)->children[1]->brk)
        {
            (*while_node)->brk = (*while_node)->children[1]->brk - 1;
            break;
        }
        else if ((*while_node)->children[1]->cont)
        {
            (*while_node)->cont = (*while_node)->children[1]->cont - 1;
            if ((*while_node)->cont != 0)
                break;
        }

        if ((*while_node)->children[0]->cond)
        {
            res = eval_do_group(&(*while_node)->children[1], env);
            if (res != -1)
                return res;
            if ((*while_node)->children[1]->brk)
            {
                (*while_node)->brk = (*while_node)->children[1]->brk - 1;
                break;
            }
            else if ((*while_node)->children[1]->cont)
            {
                (*while_node)->cont = (*while_node)->children[1]->cont - 1;
                if ((*while_node)->cont != 0)
                    break;
            }
        }
    } while ((*while_node)->children[0]->cond);
    return -1;
}

static int eval_until(struct ast **until_node, struct env **env)
{
    int res = -1;
    (*until_node)->brk = 0;
    (*until_node)->cont = 0;
    do
    {
        res = eval_do_group(&(*until_node)->children[0], env);
        if (res != -1)
            return res;
        if ((*until_node)->children[1]->brk)
        {
            (*until_node)->brk = (*until_node)->children[1]->brk - 1;
            break;
        }
        else if ((*until_node)->children[1]->cont)
        {
            (*until_node)->cont = (*until_node)->children[1]->cont - 1;
            if ((*until_node)->cont != 0)
                break;
        }
        if (!(*until_node)->children[0]->cond)
        {
            res = eval_do_group(&(*until_node)->children[1], env);
            if (res != -1)
                return res;
            if ((*until_node)->children[1]->brk)
            {
                (*until_node)->brk = (*until_node)->children[1]->brk - 1;
                break;
            }
            else if ((*until_node)->children[1]->cont)
            {
                (*until_node)->cont = (*until_node)->children[1]->cont - 1;
                if ((*until_node)->cont != 0)
                    break;
            }
        }

    } while (!(*until_node)->children[0]->cond);
    return -1;
}

static int eval_var_assign(struct ast **var_node, struct env **env)
{
    if ((*var_node)->type == AST_UNSET)
    {
        *env = env_push_direct(*env, (*var_node)->cmd.cmd, "");
        return -1;
    }
    // add 'struct env **env' as parameter
    (*var_node)->cond = 1;
    // UNCOMMENT
    (*var_node)->cmd.cmd = variableExpansion(*env, (*var_node)->cmd.cmd);
    *env = env_push_word(*env, (*var_node)->cmd.cmd);
    if ((*var_node)->type == AST_EXPORT)
        (*env)->is_export = true;
    return -1;
}

// TODO ==================================================================
// Make all functions additionally take ENV as PARAMETER
// UNCOMMENT the VARIABLE EXPANSION calls in
// eval_cmd, eval_var_assign (in this file)
// eval_pipe, eval_redirection (in file pipe_redir.c)

int eval_tree(struct ast **mom, struct env **env)
{
    int res = -1;
    for (size_t i = 0; i < (*mom)->nb_children; i++)
    {
        res = operations[(*mom)->children[i]->type](&((*mom)->children[i]), env);
        if (res != -1)
            return res;
    }
    return res;
}

// for var in a b c; do echo var; done
static int eval_for(struct ast **for_node, struct env **env)
{
    int res = -1;
    (*for_node)->brk = 0;
    (*for_node)->cont = 0;
    if ((*for_node)->children[0]->nb_children == 1)
    {
        // is there a way to know how many inputs do we have?
        //i guess we can go through the linked list until we find hashtag
        struct env *nb = env_search(*env, "#");

        int nb_arg = atoi(nb->var_value);
        for (int i = 2; i < nb_arg; i++)
        {
            char *nb_str = my_itoa(i);
            nb = env_search(*env, nb_str);
            free(nb_str);
            *env = env_push_direct(*env, (*for_node)->children[0]->children[0]->cmd.cmd, nb->var_value);
            res = eval_do_group(&(*for_node)->children[1], env);
            if (res != -1)
                return res;

            if ((*for_node)->children[1]->brk)
            {
                (*for_node)->brk = (*for_node)->children[1]->brk - 1;
                break;
            }
            else if ((*for_node)->children[1]->cont)
            {
                (*for_node)->cont = (*for_node)->children[1]->cont - 1;
                //here there is a catch. if you do a simple continue, you should 
                //not skip anything, as eval_do_group will just break itself
                if ((*for_node)->cont != 0)
                    break;
            }
        }
        *env = env_push_direct(*env, (*for_node)->children[0]->children[0]->cmd.cmd, "");
    }
    else
    {
        int flag_first = 0;
        struct env *first = env_search(*env, "karim_first");
        if (first == NULL || strcmp(first->var_value, "0") == 0)
        {
            flag_first = 1;
            *env = env_push_direct(*env, "karim_first", "1");
        }

        //size_t nb_children = (*for_node)->children[0]->nb_children;
        int adder = 0;
        for (size_t x = 1; x + adder < (*for_node)->children[0]->nb_children; x++)
        {
            char *safety = strdup((*for_node)->children[0]->children[x + adder]->cmd.cmd);
            safety = escape_ma_quotes(safety);
            char *result = variableExpansion(*env, safety);
            free((*for_node)->children[0]->children[x + adder]->cmd.cmd);
            (*for_node)->children[0]->children[x + adder]->cmd.cmd = strdup(result);
            if ((*for_node)->children[0]->children[x + adder]->cmd.cmd[0] == '"')
            {
                char *remove_q = strdup((*for_node)->children[0]->children[x + adder]->cmd.cmd);
                free((*for_node)->children[0]->children[x + adder]->cmd.cmd);
                (*for_node)->children[0]->children[x + adder]->cmd.cmd = malloc(sizeof(char));
                for (size_t i = 1; i < strlen(remove_q) -1; i++)
                {
                    (*for_node)->children[0]->children[x + adder]->cmd.cmd = \
                    realloc((*for_node)->children[0]->children[x + adder]->cmd.cmd, i * sizeof(char));
                    (*for_node)->children[0]->children[x + adder]->cmd.cmd[i - 1] = remove_q[i];
                }
                (*for_node)->children[0]->children[x + adder]->cmd.cmd = \
                realloc((*for_node)->children[0]->children[x + adder]->cmd.cmd, strlen(remove_q) * sizeof(char) -1);
                (*for_node)->children[0]->children[x + adder]->cmd.cmd[strlen(remove_q) - 2] = '\0';
                free(remove_q);
            }
            else
            {
                //size_t len = strlen((*for_node)->children[0]->children[x]->cmd.cmd);
                int nb_words = count_words(result);
                char *word = get_word(result);
                int index = 0;
                free((*for_node)->children[0]->children[x + adder]->cmd.cmd);
                (*for_node)->children[0]->children[x + adder]->cmd.cmd = strdup(word);
                free(word);

                size_t break_point = 2 + adder;
                if (adder)
                    break_point++;
                //we gotta segment each word with spaces and count how many we have
                for (int i = 1; i < nb_words; i++)
                {
                    index = get_index(result + index, index);

                    word = get_word(result + index);
                    struct ast *new_child = ast_new(AST_CMD);

                    new_child->cmd.cmd = strdup(word);

                    (*for_node)->children[0]->children = realloc((*for_node)->children[0]->children,\
                    sizeof(struct ast *) * ((*for_node)->children[0]->nb_children + 1));

                    size_t shift =(*for_node)->children[0]->nb_children;
                    (*for_node)->children[0]->children[shift] = NULL;
                    (*for_node)->children[0]->nb_children++;
                    adder++;

                    for (; shift > break_point; shift--)
                    {
                        (*for_node)->children[0]->children[shift] = \
                        (*for_node)->children[0]->children[shift -1];
                    }
                    (*for_node)->children[0]->children[break_point] = new_child;
                    break_point++;
                    free(word);
                }
            }
            free(result);
        }

        for (size_t i = 1; i < (*for_node)->children[0]->nb_children; i++)
        {
            *env = env_push_direct(*env, (*for_node)->children[0]->children[0]->cmd.cmd, 
                                (*for_node)->children[0]->children[i]->cmd.cmd);
            res = eval_do_group(&(*for_node)->children[1], env);
            if (res != -1)
                return res;
            if ((*for_node)->children[1]->brk)
            {
                (*for_node)->brk = (*for_node)->children[1]->brk - 1;
                break;
            }
            else if ((*for_node)->children[1]->cont)
            {
                (*for_node)->cont = (*for_node)->children[1]->cont - 1;
                //here there is a catch. if you do a simple continue, you should 
                //not skip anything, as eval_do_group will just break itself
                if ((*for_node)->cont != 0)
                {
                    if (flag_first)
                        continue;
                    else
                        break;
                }
            }
        }
        if (flag_first == 1)
        {
            *env = env_push_direct(*env, "karim_first", "0");
        }
        *env = env_push_direct(*env, (*for_node)->children[0]->children[0]->cmd.cmd, "");
    }
    return res;
}

static int eval_continue_break(struct ast **cont_node, struct env **env)
{
    cont_node = cont_node;
    env = env;
    return -1;
}

static int eval_cd(struct ast **cd_node, struct env **env)
{
    env = env;
    //remember ti run this in a subshell only
    //also error grammar checking
    int return_value = -1;
    if (strcmp((*cd_node)->cmd.cmd, "-" ) == 0)
    {
        //upon booting up, if they immediately execute this
        //then getenv oldpwd should return NULL 
        //so nothing will happen
        return_value = chdir(getenv("OLDPWD"));
    }
    else if (strcmp((*cd_node)->cmd.cmd, ".." ) == 0)
    {
        //max length of (*cd_node)->cmd.cmdm needs to be checked!
        char new_path[1024];
        strcpy(new_path, getenv("PWD"));

        //match the last / character in $PWD
        char *last_slash = strrchr(new_path, '/');
        //null terminate the (*cd_node)->cmd.cmd string at the last / character 
        *last_slash = '\0';
        //chdir == change directory
        return_value = chdir(new_path);    
    }
    else if (strcmp((*cd_node)->cmd.cmd, "." ) == 0)
    {
        return_value = chdir(getenv("PWD"));
    }
    else if((*cd_node)->cmd.cmd[0] != '/')//relative (*cd_node)->cmd.cmd
    {
        char new_path [1024];
        strcpy(new_path,getenv("PWD"));
        strcat(new_path,"/");
        strcat(new_path, (*cd_node)->cmd.cmd);
        return_value = chdir(new_path);
    }
    else
    {
        return_value = chdir((*cd_node)->cmd.cmd);
    }
    
    if (return_value == -1)
    {
        fprintf(stderr,"cd: %s: No such file or directory\n", (*cd_node)->cmd.cmd);
    }
    else//update the $OLDPWD and $PWD environment variables on success of chdir() system call
    {
        char cwd[1024];
        getcwd(cwd, 1024);
        //not sure if i should also change stuff in struct env *env
        setenv("OLDPWD",getenv("PWD"),1); //update $OLDPWD to the value of $PWD
        setenv("PWD",cwd,1); //update $PWD to the current directory 
    }
    return -1;
}

static int eval_export(struct ast **export_node, struct env **env)
{
    //remember ti run this in a subshell only
    char *var = strdup((*export_node)->cmd.cmd);
    char *value = NULL;
    size_t len = strlen(var);
    size_t i = 0;
    while (i < len)
    {
        if ((*export_node)->cmd.cmd[i] == '=')
        {
            var[i] = '\0';
            var = realloc(var, sizeof(char) * (i + 1));
            i++;
            break;
        }
        i++;
    }
    //copy the rest of variable
    if (i != len)
    {
        value = strdup((*export_node)->cmd.cmd + i);
    }
    //then we need to put them in the env struct under the banner of
    //EXPORTED VAR
    //this way , when we make a new struct env for the subshell , we go through the
    //current env struct and immediately push these EXPORTED tagged variable
    *env = env_push_direct(*env, var, value);
    (*env)->is_export = true;
    return -1;
}

static int eval_exit(struct ast **exit_node, struct env **env)
{
    //this quits everything
    //0 is the EXIT_SUCCESS value
    //NULL so that stderr has nothing printed
    exit_node = exit_node;
    env = env;
    return (*exit_node)->fd;
}

static int eval_dot(struct ast **dot_node, struct env **env)
{
    int res = -1;
    FILE *file;
    // . ./$example but could we actually have this case?
    //i will assume that we can...
    char *safety = strdup((*dot_node)->cmd.cmd);
    char *result = variableExpansion(*env, safety);
    if (result == NULL)
    {
        free(safety);
        file = fopen((*dot_node)->cmd.cmd, "r");
    }
    else
    {
        free((*dot_node)->cmd.cmd);
        (*dot_node)->cmd.cmd = strdup(result);
        free(result);
        file = fopen((*dot_node)->cmd.cmd, "r");
    }
    char buf[10000];
    fread(buf, sizeof(char), 1000, file);
    struct lexer *dot_lex = lexer_new(buf);
    enum parser_status status = parse_input(dot_node, dot_lex, NULL);
    while (status == PARSER_CONTINUE)
    {
        token_free(lexer_pop(dot_lex));
        status = parse_input(dot_node, dot_lex, NULL);
    }
    //lexer_free(dot_lex);
    fclose(file);
    for (size_t i = 0; i < (*dot_node)->nb_children; i++)
    {
        res = operations[(*dot_node)->children[0]->type]\
        (&((*dot_node)->children[0]), env);
    }
    return res;
}

static int eval_unset(struct ast **unset_node,struct env **env)
{
    *env = env_push_direct(*env, (*unset_node)->cmd.args, "");
    return -1;
}