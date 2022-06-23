#ifndef VARIABLE_EXPANSION_H
#define VARIABLE_EXPANSION_H

#include "env.h"
#include "index.h"

char *variableProcessing(struct env *env, char *word);
char *variableExpansion(struct env *env, char *word);

#endif