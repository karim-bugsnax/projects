#ifndef VARIABLE_EXPANSION_SYNTAX_H
#define VARIABLE_EXPANSION_SYNTAX_H

#include "index.h"
#include <stdbool.h>

int checkSpecial(char *word, struct index *index);
int checkSyntax(char *word, struct index *index, int wordIndex);
bool checkQuoteSyntax(char *word);

#endif