#ifndef VARIABLE_EXPANSION_PROCESSING_H
#define VARIABLE_EXPANSION_PROCESSING_H

#include "env.h"
#include "index.h"

char *commandProcessing(char *result, char *word, struct index *index);
char *escapeProcessing(char *result, char *word, struct index *index);
char *singleQuoteProcessing(char *result, char *word, struct index *index);
char *doubleQuoteProcessing(char *result, struct env *env, char *word, struct index *index);

#endif 