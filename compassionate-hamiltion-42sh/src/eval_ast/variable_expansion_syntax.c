#define _XOPEN_SOURCE 500

#include "variable_expansion_syntax.h"

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
#include "variable_expansion_expansion.h"
#include "variable_expansion_processing.h"


/**
 * @brief check if a character is a special character, get random value if $RANDOM
 * @param word 
 * @param index dollarIndex defined
 * @return int 1 if yes, -1 if not, startIndex, update Index
 */
int checkSpecial(char *word, struct index *index)
{
    index->startIndex = index->wordIndex;
    index->lastIndex = index->startIndex;

    if (word[index->startIndex] == '@' || word[index->startIndex] == '*' || word[index->startIndex] == '?'
    || word[index->startIndex] == '$' || word[index->startIndex] == '#')
    {
        index->lastIndex += 1;
        index->wordIndex += 1;
        return 1;
    }
    // form 1$RANDOM => $RANDOM1 error case
    if (word[index->startIndex] == 'R' || word[index->startIndex] == 'U' || word[index->startIndex] == 'O'
    || word[index->startIndex] == 'I')
    {
        while(word[index->lastIndex] != '\0')
            index->lastIndex += 1;
        
        if (word[index->lastIndex] == '\0') // $RANDOM case => special case
        {
            index->finalIndex = index->lastIndex;

            char *segment = cutOutSegment(word, index->wordIndex, index->lastIndex - 1);
            if (strcmp(segment, "RANDOM") == 0)
            {
                free(segment);
                index->randomValue = my_itoa(rand() % (32766));
                return 1;
            }
            if ((strcmp(segment, "UID") == 0 || strcmp(segment, "OLDPWD") == 0 || strcmp(segment, "IFS") == 0))
            {
                free(segment);
                return 1;
            }
        }
        else // not special case
        {
            index->lastIndex = index->startIndex;
            return -1;
        }
    }

    return -1;
}

/**
 * @brief checks if the syntax of ONE variable segment is valid
 * updates startIndex and lastIndex to the beginning of varName, if ERROR, increment
 * updates dollarIndex and finalIndex to the $ and } index, if ERROR, increment
 * @param word 
 * @param index startIndex = $ index 
 * @return int 3 if command substitution 
 *             2 if special characters $# $@ $! $~ etc
 *             1 if valid $A ${A} forms
 *             0 if other characters, just printing
 *             -1 if bad substitution/ERROR
                -2 if S" S' case => do not skip $, print $
 */
int checkSyntax(char *word, struct index *index, int wordIndex)
{
    // check if $ is the last character => 0 printing
    if (word[wordIndex + 1] == '\0' || word[wordIndex + 1] == '\\')
        return 0;
    
    if (word[wordIndex + 1] != '\0' && (word[wordIndex + 1] == '\"' || word[wordIndex + 1] == '\''))
        return -2;

    if (word[wordIndex] == '`')
    {
        if (word[wordIndex + 1] == '\0')
            return -1;

        wordIndex += 1;
        index->startIndex = wordIndex;

        while (word[wordIndex] != '`')
            wordIndex += 1;
        if (wordIndex == index->wordLength - 1 && word[wordIndex] != '`')
            return -1; // No ending `, ERROR
        index->wordIndex = wordIndex;
        index->lastIndex = wordIndex - 1;

        return 3;
    }
    
    index->dollarIndex = wordIndex;

    for (int i = index->dollarIndex + 1; i < index->wordLength; i++)
    {
        if (word[i] == '}' || word[i] == ')')
            return 0;
        // variable expansion
        else if (word[i] == '{') // form ${A}
        {
            if (word[i + 1] == '}')
                return -1; // bad substitution, ERROR case
            
            i += 1;
            index->startIndex = i;

            while (word[i] != '}')
            {
                if (word[i] == '{' || isspace(word[i]) != 0 || (i == index->wordLength - 1 && word[i] != '}')) // form ${A or ${A{ => ERROR
                    return -1; 
                i += 1;
            }
            index->lastIndex = i - 1;
            index->finalIndex = i;
            index->wordIndex = i;
            break;
        }
        // command substitution
        else if (word[i] == '(') // form $(A)
        {
            if (word[i + 1] == ')')
                return -1; // bad substitution, ERROR case
            
            i += 1;
            index->startIndex = i;

            while (word[i] != ')')
            {
                if (word[i] == '(' || isspace(word[i]) != 0 || (i == index->wordLength - 1 && word[i] != '}')) // form $(A or $(A()=> ERROR
                    return -1; 
                i += 1;
            }
            index->lastIndex = i - 1;
            index->finalIndex = i;
            index->wordIndex = i;
            return 3;
        }
        
        else if (isalnum(word[i]) != 0 || ispunct(word[i]) != 0) // form $A
        {
            index->wordIndex = i;
            // check if special case
            int isSpecial = checkSpecial(word, index);
            if (isSpecial != -1) 
                return 2;

            // check if form $A
            while (isalnum(word[i]) != 0 || word[i] == '_')
            {
                index->wordIndex += 1;
                i += 1;
            }
            index->wordIndex -= 1;
            i = index->wordIndex;
            index->lastIndex = i;
            index->finalIndex = i;
            break;
        }
        else // variable containing forbidden character => print
            return 0;
    }
    return 1;
}

bool checkQuoteSyntax(char *word)
{
    int doubleQuote = 0, singleQuote = 0;
    for (size_t i = 0; i < strlen(word); i++)
    {
        if (i == 0 && word[i] == '\"')
            doubleQuote += 1;
        if (i == 0 && word[i] == '\'')
            singleQuote += 1;
        if (i >= 1 && word[i] == '\"' && word[i - 1] != '\\')
            doubleQuote += 1;
        if (i >= 1 && word[i] == '\'' && word[i - 1] != '\\')
            singleQuote += 1;
    }

    if (doubleQuote > 0 && singleQuote == 0)
        return (doubleQuote % 2 == 0);
    if (doubleQuote == 0 && singleQuote > 0)
        return (singleQuote % 2 == 0);
    return ((doubleQuote % 2 == 0) || (singleQuote % 2 == 0));
}
