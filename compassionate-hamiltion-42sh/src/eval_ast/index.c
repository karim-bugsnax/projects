#include "index.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct index *createIndex(char *word)
{
    struct index *index = malloc(sizeof(struct index));
    index->startIndex = 0;
    index->lastIndex = 0;
    index->dollarIndex = 0;
    index->finalIndex = 0;
    index->resultIndex = 0;
    index->wordIndex = 0;
    index->randomValue = NULL;

    size_t len = strlen(word);
    int wordLength = len;

    index->wordLength = wordLength;
    index->next = NULL;

    return index;
}

void printIndex(struct index *index)
{
    printf("startIndex: %d lastIndex: %d dollarIndex: %d finalIndex: %d resultIndex: %d wordIndex: %d wordLength: %d\n", 
    index->startIndex, index->lastIndex, index->dollarIndex, index->finalIndex, index->resultIndex, index->wordIndex, index->wordLength);
}