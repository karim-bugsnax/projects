#include <err.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>
#include "env.h"

void exit_builtin(int exit_status)
{
    //0 is the EXIT_SUCCESS value
    //NULL so that stderr has nothing printed
    errx(0, NULL);
}

/**
 * @brief implementatio nof the CD command
 * 
 * @param path an array of string : cd - pathname or cd pathname
 * @param env the env struct we use
 * @return int 
 */
int cd_builtin(char **path, struct env **env)
{
    int return_value = -1;
    if (strcmp(path[1], "-" ) == 0)
    {
        //upon booting up, if they immediately execute this
        //then getenv oldpwd should return NULL 
        //so nothing will happen
        return_value = chdir(getenv("OLDPWD"));
    }
    else if (strcmp(path[1], ".." ) == 0)
    {
        //max length of pathm needs to be checked!
        char new_path[1024];
        strcpy(new_path, getenv("PWD"));

        //match the last / character in $PWD
        char *last_slash = strrchr(new_path, '/');
        //null terminate the path string at the last / character 
        *last_slash = '\0';
        //chdir == change directory
        return_value = chdir(new_path);    
    }
    else if (strcmp(path[1], "." ) == 0)
    {
        return_value = chdir(getenv("PWD"));
    }
    else if(path[1][0] != '/')//relative path
    {
        char new_path [1024];
        strcpy(new_path,getenv("PWD"));
        strcat(new_path,"/");
        strcat(new_path, path[1]);
        return_value = chdir(new_path);
    }
    else
    {
        return_value = chdir(path[1]);
    }
    
    if (return_value == -1)
    {
        fprintf(stderr,"cd: %s: No such file or directory\n", path[1]);
    }
    else//update the $OLDPWD and $PWD environment variables on success of chdir() system call
    {
        char cwd[1024];
        getcwd(cwd, 1024);
        //not sure if i should also change stuff in struct env *env
        setenv("OLDPWD",getenv("PWD"),1); //update $OLDPWD to the value of $PWD
        setenv("PWD",cwd,1); //update $PWD to the current directory 
    }
}


/**
 * @brief remove variable and/ or func defs from env structure
 * 
 * @param env : our structure
 * @param variable : unset [-fv] name  // unset -v varkarim unset -f funcshrex
 */
void unset_builtin(struct env **env, char *variable)
{
    //i guess we will need to modify our structure to also hold func defs...
    *env = env_push_direct(*env, variable, "");
}

//for implementing cotninue and break : 
//this is a trick one that should be dealt with in the eval_tree
//essentially, when going down our tree, we must count the number of loops, and indented loops
//everytime we see a continue, we must jump all the way up to that node, and run again.....
// now how to do that....I HAVE NO FUCKING IDEA



void dot_builtin(char *filename) 
{
    // open the file 
    FILE *file = fopen(filename, "r");
    // get line 
    
    // implement dot 
    char buf[10000];
    fread(buf, sizeof(char), 1000, fd);
    lex = lexer_new(buf);
    mom = ast_new(AST_MOM);
    enum parser_status status = parse_input(&mom, lex);
    while (status == PARSER_CONTINUE)
    {
        status = parse_input(&mom, lex);
    }
    res = eval_tree(&mom, &env); 
    lexer_free(lex);
    ast_free(mom);
    env_free(env);

    fclose(file);
}