#define _XOPEN_SOURCE 500

#include "variable_expansion_processing.h"

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
#include "variable_expansion_expansion.h"

char *commandProcessing(char *result, char *word, struct index *index)
{
    // get the command 
    char *command = cutOutSegment(word, index->startIndex, index->lastIndex);
    // subshell 
    char *subshellRes = subshell(command);
    free(command);

    if (subshellRes == NULL)
        return NULL;
    
    result = expansion(result, subshellRes, index);
    free(subshellRes);

    if (result == NULL)
        return NULL;
    
    return result;
}

char *escapeProcessing(char *result, char *word, struct index *index)
{
    if (word[index->wordIndex + 1] != '\0')
    {
        result[index->resultIndex] = word[index->wordIndex + 1];
        index->resultIndex += 1;
        result[index->resultIndex] = '\0';

        index->wordIndex += 1;

        return result;
    }
    else // ERROR case
        return NULL;
}

char *singleQuoteProcessing(char *result, char *word, struct index *index)
{
    index->wordIndex += 1;
    if (word[index->wordIndex] == '\'')
        return result;

    while (word[index->wordIndex] != '\'')
    {
        result[index->resultIndex] = word[index->wordIndex];
        index->resultIndex += 1;
        index->wordIndex += 1;
    }
    
    result[index->resultIndex] = '\0';
    return result;
}

char *doubleQuoteProcessing(char *result, struct env *env, char *word, struct index *index)
{
    index->wordIndex += 1;
    if (word[index->wordIndex] == '\"')
    {
        index->startIndex = index->wordIndex + 1;
        index->lastIndex = index->startIndex;
        return result; // nothing added
    }

    int isSyntaxValid = 0;
    int i;
    for (i = index->wordIndex; i < index->wordLength && word[i] != '\"'; i++)
    {
        index->wordIndex = i;

        if (word[i] == '\\')
        {
            result = escapeProcessing(result, word, index);
            if (result == NULL)
                break; // waiting > case
            i = index->wordIndex;
        }

        else if (word[i] == '\'')
        {
            result[index->resultIndex] = word[i];
            index->resultIndex += 1;
        }

        else if (word[i] == '$')
        {
            // checks valid syntax, startIndex, lastIndex update to varName
            isSyntaxValid = checkSyntax(word, index, i); 

            if (isSyntaxValid == 1 || isSyntaxValid == 2) // variable expansion, special case
            {
                // expanded string of ONE segment
                result = expandVariable(result, env, word, index);
                if (result == NULL) // not defined variable, return NULL
                    return NULL;
                i = index->finalIndex;
            }
            else if (isSyntaxValid == 0) // print case
            {
                result[index->resultIndex] = word[i];
                index->resultIndex += 1;
            }
            else if (isSyntaxValid == -1) // print, bad substitution case
                break;

            else if (isSyntaxValid == -2) // $' case => skip $
            {
                // add to the result
                result[index->resultIndex] = '$';
                index->resultIndex += 1;
                result[index->resultIndex] = '\0';
                index->startIndex = i + 1;
                index->lastIndex = i + 1;
                index->wordIndex = i + 1;
            }
        }
        // if not variable expansion case, append into result string
        else if (word[i] != '$') 
        {
            result[index->resultIndex] = word[i];
            index->resultIndex += 1;
            result[index->resultIndex] = '\0';
        }
    }
    // the last double quote 
    index->wordIndex = i;
    index->lastIndex = i;

    result[index->resultIndex] = '\0';

    return result;
}