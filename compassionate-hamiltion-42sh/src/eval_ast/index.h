#ifndef INDEX_H
#define INDEX_H

struct index
{
    int startIndex;
    int lastIndex;
    int dollarIndex;
    int finalIndex;
    int resultIndex;
    int wordIndex;
    int wordLength;
    char *randomValue;
    struct index *next;
};

struct index *createIndex(char *word);
void printIndex(struct index *index);

#endif