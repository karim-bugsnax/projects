#define _XOPEN_SOURCE 500

#include "variable_expansion.h"

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
#include "variable_expansion_processing.h"


/**
 * @brief detect $ and check if it is a variable expanding case 
 * if yes, perform variable expansion
 * if not, return NULL
 * @param env 
 * @param word 
 * @return char* final result of ALL variables expanded string, NULL if undefined 
 */
char *variableProcessing(struct env *env, char *word)
{
    bool isQuoteValid = checkQuoteSyntax(word);
    if (isQuoteValid == false)
        return NULL; // ERROR case

    char *result = malloc(sizeof(char) * 1000 + 1);
    result[1000] = '\0';
    struct index *index = createIndex(word);

    for(int i = 0; i < index->wordLength; i++)
    {
        int isSyntaxValid = 0;
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
            result = singleQuoteProcessing(result, word, index);
            i = index->wordIndex;
        }
        else if (word[i] == '\"')
        {
            result = doubleQuoteProcessing(result, env, word, index);
            i = index->wordIndex;
        }
        else if (word[i] == '`')
        {
            isSyntaxValid = checkSyntax(word, index, i);
            if (isSyntaxValid != 3)
                return NULL;
            result = commandProcessing(result, word, index);
            i = index->wordIndex;
        }
        // detect $
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
            if (isSyntaxValid == 3) // command substitution
            {
                result = commandProcessing(result, word, index);
                if (result == NULL) // not defined variable, return NULL
                    return NULL;
                i = index->wordIndex;
            }
            else if (isSyntaxValid == 0) // print case
            {
                result[index->resultIndex] = word[i];
                index->resultIndex += 1;
                result[index->resultIndex] = '\0';
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

    result[index->resultIndex] = '\0';
    if (index->randomValue != NULL)
        free(index->randomValue);
    free(index);
    return result;
}

/**
 * @brief perform variable expansion and escape processing to cancel backslash and single/double quotes
 * @param env linked list of variable assignments
 * @param word
 * @return char* expanded string, if undefined variable or an error case, return NULL
 */
char *variableExpansion(struct env *env, char *word)
{
    char *result = variableProcessing(env, word);
    if (result == NULL)
        return NULL; // ERROR case

    free(word);
    return result;
}