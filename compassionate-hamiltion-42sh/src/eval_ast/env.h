#ifndef ENV_H
#define ENV_H

#include <stdbool.h>

struct env
{
    // x = 3
    char *var_name; // x
    char *var_value; // 3
    struct env *next;
    bool is_export;
};

struct env *env_push_word(struct env *env, char *var_assign);
struct env *env_push_direct(struct env *env, char *var_name, char *var_value);
struct env *env_search(struct env *env, char *var_name);
void env_free(struct env *env);
char *my_itoa(int value);
struct env *init_env(int argc, char *argv[]);
struct env *env_get_export(struct env *env);

#endif