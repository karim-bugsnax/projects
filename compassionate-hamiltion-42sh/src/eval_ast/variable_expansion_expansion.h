#ifndef VARIABLE_EXPANSION_EXPANSION_H
#define VARIABLE_EXPANSION_EXPANSION_H

#include "env.h"
#include "index.h"

char *cutOutSegment(char *word, int start, int last);
char *subshell(char *cmd);
char *getVarValue(struct env *env, char *word, struct index *index);
char *getCommand(char *varValue);
char *expansion(char *result, char *varValue, struct index *index);
char *expandVariable(char *result, struct env *env, char *word, struct index *index);

#endif