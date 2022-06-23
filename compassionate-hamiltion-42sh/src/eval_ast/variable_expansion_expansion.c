#define _XOPEN_SOURCE 500

#include "variable_expansion_expansion.h"

#include <ctype.h>
#include <err.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include "env.h"
#include "index.h"
#include "variable_expansion_syntax.h"
#include "variable_expansion_processing.h"

/**
 * @brief cuts out the string from the given word from startIndex to lastIndex
 * but excluding brackets
 * @return char* cut out segment
 */
char *cutOutSegment(char *word, int start, int last)
{
    size_t len = strlen(word);
    char *segment = malloc(sizeof(char) * len + 1);
    segment[len] = '\0';
    
    int segmentIndex = 0;
    for (int i = start; i <= last; i++)
    {
        if (word[i] == '{')
            continue;
        else if (word[i] == '}')
            break;
        else
        {
            segment[segmentIndex] = word[i];
            segmentIndex += 1;
        }
    }
    segment[segmentIndex] = '\0';

    return segment;
}

char **split_cmd(char *in)
{
    int len = strlen(in);
    char **res = malloc(sizeof(char*) * len);
    int res_i = 0;
    int i = 0;
    
    while(i < len)
    {
        int start = i;
        while (in[i] != ' ' && in[i] != '\t' && in[i] != '\0')
            i++;
        in[i] = '\0';
        i += 1;

        res[res_i] = in + start;
        res_i += 1;

        while (in[i] == ' ' || in[i] == '\t')
            i++;
    }

    res[res_i] = NULL;
    return res;
}

char *subshell(char *cmd)
{
    char **args = split_cmd(cmd);
    int first_fds[2]; // 0 = read, 1 = write
    if (pipe(first_fds) == -1)
        errx(1, "Failed to create pipe file descriptors.");

    int stdout_dup = dup(STDOUT_FILENO);
    if (dup2(first_fds[1], STDOUT_FILENO) == -1)
        errx(1, "Dup2 error.");

    int pid = fork();
    if (pid == -1)
        errx(1, "Could not fork\n");
    else if (pid == 0) // Child (in which we fork again)
    {
        close(first_fds[0]);
        if (execvp(args[0], args) == -1)
            errx(127, "Execvp could not be executed.");
    }
    else // Main Parent
    {
        int wstatus;
        if (waitpid(pid, &wstatus, 0) == -1)
            errx(1, "Error, no args or failed execvp\n");
    }

    free(args);
    fflush(stdout);
    if (dup2(stdout_dup, STDOUT_FILENO) == -1)
        errx(1, "Dup2 error.");

    char *res = malloc(sizeof(char) * (1000) + 1);

    int count = read(first_fds[0], res, 1000);
    res[count - 1] = '\0';

    close(first_fds[1]);
    close(first_fds[0]);

    close(stdout_dup);

    return res;
}

/**
 * @brief Get the Var Value object
 * 
 * @param env 
 * @param word 
 * @param index startIndex, lastIndex defined
 * @return char* varValue of ONE segment
 */
char *getVarValue(struct env *env, char *word, struct index *index)
{
    char *varName = cutOutSegment(word, index->startIndex, index->lastIndex);

    struct env *element = env_search(env, varName);
    if (element == NULL) // undefined variable, return NULL
    {
        free(varName);
        return "";
    }

    free(varName);
    return element->var_value;
}

/**
 * @brief append varValue into result
 * 
 * @param result 
 * @param varValue 
 * @return char* appended result string
 */
char *expansion(char *result, char *varValue, struct index *index)
{
    for (size_t i = 0; i < strlen(varValue); i++, index->resultIndex++)
        result[index->resultIndex] = varValue[i];

    result[index->resultIndex] = '\0';

    return result;
}

/**
 * @brief find variable value in env list, update dollar and final index, expand onto result
 * 
 * @param result empty
 * @param env 
 * @param word 
 * @param index start and lastIndex on varName; dollar and finalIndex updated 
 * @return char* variable expanded result for ONE segment, NULL if undefined
 */
char *expandVariable(char *result, struct env *env, char *word, struct index *index)
{
    char *varValue;
    // $RANDOM case
    if (index->randomValue != NULL)
        varValue = index->randomValue;
    else
        varValue = getVarValue(env, word, index);

    // expansion
    result = expansion(result, varValue, index);
    if (result == NULL) 
        return NULL;
    
    return result;
}

