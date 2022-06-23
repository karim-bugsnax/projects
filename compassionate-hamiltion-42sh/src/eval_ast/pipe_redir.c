#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../ast/ast.h"
#include "env.h"
#include "variable_expansion.h"

int exec_fork(char **args)
{
    int pid = fork();
    if (pid == -1)
        errx(1, "Could not fork\n");
    else if (pid == 0) // Child (in which we fork again)
    {
        if (execvp(args[0], args) == -1)
            errx(127, "Execvp could not be executed.");
    }
    else // Main Parent
    {
        int wstatus;
        if (waitpid(pid, &wstatus, 0) == -1)
            errx(1, "Error, no args or failed execvp\n");
    }

    return 0;
}

// Main pipe function
void eval_pipe_aux(char **cmd_arr, struct ast **pipe_node, struct env **env)
{
    env = env;
    size_t len = (*pipe_node)->nb_children;

    // copy stdout and stdin & init of fds
    int stdout_dup = dup(STDOUT_FILENO);
    int stdin_dup = dup(STDIN_FILENO);

    int first_fds[2]; // 0 = read, 1 = write
    if (pipe(first_fds) == -1)
        errx(1, "Failed to create pipe file descriptors.");

    // first exec
    if (dup2(first_fds[1], STDOUT_FILENO) == -1)
        errx(1, "Dup2 error.");

    exec_fork(cmd_arr);

    // handle all execs between first and last
    size_t count = 1;
    while (count < len - 1)
    {
        int fds[2]; // 0 = read, 1 = write
        if (pipe(fds) == -1)
            errx(1, "Failed to create pipe file descriptors.");
        
        close(first_fds[1]);
        if (dup2(first_fds[0], STDIN_FILENO) == -1)
            errx(1, "Dup2 error.");
        if (dup2(fds[1], STDOUT_FILENO) == -1)
            errx(1, "Dup2 error.");

        exec_fork((*pipe_node)->children[count]->children[0]->cmd.cmd_arr);

        close(first_fds[0]);
        
        first_fds[0] = fds[0];
        first_fds[1] = fds[1];
        count += 1;
    }

    // last exec
    close(first_fds[1]);
    if (dup2(first_fds[0], STDIN_FILENO) == -1)
        errx(1, "Dup2 error.");
    if (dup2(stdout_dup, STDOUT_FILENO) == -1)
        errx(1, "Dup2 error.");

    exec_fork((*pipe_node)->children[count]->children[0]->cmd.cmd_arr); // count == len - 1

    fflush(stdout);
    close(first_fds[0]);

    if (dup2(stdin_dup, STDIN_FILENO) == -1)
        errx(1, "Dup2 error.");

    close(stdout_dup);
    close(stdin_dup);
}

// In this function we expand all necessary strings, and handle command group / single cmd case
int eval_pipe(struct ast **pipe_node, struct env **env)
{
    (*pipe_node)->cond = 1;

    if ((*pipe_node)->children[0]->type == AST_NORED)
    {
        // expand all cmd.cmd_arr of children of pipe_node
        for (size_t j = 0; j < (*pipe_node)->nb_children; j++)
        {
            for(size_t i = 0; (*pipe_node)->children[j]->children[0]->cmd.cmd_arr[i] != NULL; i++)
                (*pipe_node)->children[j]->children[0]->cmd.cmd_arr[i] =
                 variableExpansion(*env, (*pipe_node)->children[j]->children[0]->cmd.cmd_arr[i]);
        }

        eval_pipe_aux((*pipe_node)->children[0]->children[0]->cmd.cmd_arr, pipe_node, env);
    }
    else // case: we have a group cmd (e.g. {echo a; echo b;}, so we need to access all children of that)
    {
        // expand all cmd.cmd_arr of children of pipe_node (except first)
        for (size_t j = 1; j < (*pipe_node)->nb_children; j++)
        {
            for(size_t i = 0; (*pipe_node)->children[j]->cmd.cmd_arr[i] != NULL; i++)
           // for(size_t i = 0; (*pipe_node)->children[j]->cmd.index > i; i++)
                (*pipe_node)->children[j]->cmd.cmd_arr[i] =
                 variableExpansion(*env, (*pipe_node)->children[j]->cmd.cmd_arr[i]);
        }

        // expand cmd.cmd_arr of children of first child of pipe_node
        for (size_t j = 0; j < (*pipe_node)->children[0]->nb_children; j++)
        {
            for(size_t i = 0; (*pipe_node)->children[0]->children[j]->cmd.cmd_arr[i] != NULL; i++)
            //for(size_t i = 0; (*pipe_node)->children[0]->children[j]->cmd.index < i; i++)
                (*pipe_node)->children[0]->children[j]->cmd.cmd_arr[i] =
                 variableExpansion(*env, (*pipe_node)->children[0]->children[j]->cmd.cmd_arr[i]);
            
            // apply pipe with each child
            eval_pipe_aux((*pipe_node)->children[0]->children[j]->cmd.cmd_arr, pipe_node, env);
        }
    }
    return -1;
}

int eval_redirection(struct ast **redir_node, struct env **env)
{
    (*redir_node)->cond = 1;
    // Expand all strings in the cmd node (first node)
    for(int i = 0; (*redir_node)->children[0]->cmd.cmd_arr[i] != NULL; i++)
        (*redir_node)->children[0]->cmd.cmd_arr[i] =
         variableExpansion(*env, (*redir_node)->children[0]->cmd.cmd_arr[i]);

    // Saving Standard FDs
    int stdinDup = dup(STDIN_FILENO);
    int stdoutDup = dup(STDOUT_FILENO);
    int stderrDup = dup(STDERR_FILENO);

    // Loop over all children and set input / output
    for(size_t i = 1; i < (*redir_node)->nb_children; i++)
    {
        // Get info
        struct ast *curr = (*redir_node)->children[i];
        char *redir_symbol = curr->cmd.cmd;
        // Expand filename
        curr->children[1]->cmd.cmd =
         variableExpansion(*env, curr->children[1]->cmd.cmd);
        char *file = curr->children[1]->cmd.cmd;
        // if RED_R, use STDOUT as default, else STDIN
        int ion = curr->type == AST_REDR ? STDOUT_FILENO : STDIN_FILENO;
        if (curr->children[0]->fd != -1)
            ion = curr->children[0]->fd;

        int fd;
        // Open fds according to redir_symbol
        if (strcmp(redir_symbol, ">>") == 0)
            fd = open(file, O_CREAT | O_APPEND | O_WRONLY, 0755);
        else if (strcmp(redir_symbol, ">&") == 0 || strcmp(redir_symbol, "<&") == 0)
        {
            if (strcmp(file, "-") == 0)
            {
                close(file[0] - '0');
                fd = ion;
            }
            else
                fd = dup(file[0] - '0');
        }
        else if (strcmp(redir_symbol, "<>") == 0)
            fd = open(file, O_CREAT | O_RDWR, 0755);
        else // symbol == > or >| or <
            fd = open(file, O_CREAT | O_TRUNC | O_WRONLY, 0755);

        if (fd < 0)
            errx(1, "ERROR: failure in opening the file.");
        curr->children[0]->fd = fd;

        // Redirect the IO_NUMBER to the correct fd
        if (dup2(fd, ion) == -1)
            errx(1, "ERROR: failure in dup2().");
    }
    
    // exec cmd (handle echo case) or cat file (first node only)
    if ((*redir_node)->children[0]->nb_children == 0)
    {
        // special handle for ECHO
        if (strcmp((*redir_node)->children[0]->cmd.cmd_arr[0], "echo") == 0)
        {
            for(int i = 1; (*redir_node)->children[0]->cmd.cmd_arr[i] != NULL; i++)
                puts((*redir_node)->children[0]->cmd.cmd_arr[i]);
        }
        else
            exec_fork((*redir_node)->children[0]->cmd.cmd_arr);
    }
    else
    {
        char *filename = (*redir_node)->children[0]->children[0]->cmd.cmd_arr[0];
        char *args[] = {"cat", filename, NULL};
        exec_fork(args);
    }

    // close all opened files
    for(size_t i = 1; i < (*redir_node)->nb_children - 1; i++)
        close((*redir_node)->children[i]->children[0]->fd);

    // Close and clean everything
    fflush(stdout);
    dup2(stdinDup, STDIN_FILENO);
    dup2(stdoutDup, STDOUT_FILENO);
    dup2(stderrDup, STDERR_FILENO);
    close(stdinDup);
    close(stdoutDup);
    close(stderrDup);
    return -1;
}

/*
#include <string.h>
#include <stdlib.h>
int main(void)
{
    if (100 == 1)
        eval_pipe(NULL, NULL);

    struct ast *mom = malloc(sizeof(struct ast));
    mom->nb_children = 3;
    mom->children = malloc(sizeof(struct ast*) * 3);

    mom->children[0] = malloc(sizeof(struct ast));
    mom->children[0]->nb_children = 0;
    mom->children[0]->cmd.cmd_arr = malloc(sizeof(char*) * 2);
    mom->children[0]->cmd.cmd_arr[0] = malloc(sizeof(char) * 20);
    //mom->children[0]->cmd.cmd_arr[1] = malloc(sizeof(char) * 20);
    strcpy(mom->children[0]->cmd.cmd_arr[0], "ls");
    //strcpy(mom->children[0]->cmd.cmd_arr[1], NULL);
    mom->children[0]->cmd.cmd_arr[1] = NULL;

    mom->children[1] = malloc(sizeof(struct ast));
    mom->children[1]->type = AST_REDR;
    mom->children[1]->nb_children = 2;
    mom->children[1]->cmd.cmd = malloc(sizeof(char) * 5);
    strcpy(mom->children[1]->cmd.cmd, ">");
    mom->children[1]->children = malloc(sizeof(struct ast*) * 2);
    mom->children[1]->children[0] = malloc(sizeof(struct ast));
    mom->children[1]->children[0]->fd = -1;
    mom->children[1]->children[1] = malloc(sizeof(struct ast));
    mom->children[1]->children[1]->cmd.cmd = malloc(sizeof(char) * 20);
    strcpy(mom->children[1]->children[1]->cmd.cmd, "file3");

    mom->children[2] = malloc(sizeof(struct ast));
    mom->children[2]->type = AST_REDR;
    mom->children[2]->nb_children = 2;
    mom->children[2]->cmd.cmd = malloc(sizeof(char) * 5);
    strcpy(mom->children[2]->cmd.cmd, ">&");
    mom->children[2]->children = malloc(sizeof(struct ast*) * 2);
    mom->children[2]->children[0] = malloc(sizeof(struct ast));
    mom->children[2]->children[0]->fd = 2;
    mom->children[2]->children[1] = malloc(sizeof(struct ast));
    mom->children[2]->children[1]->cmd.cmd = malloc(sizeof(char) * 20);
    strcpy(mom->children[2]->children[1]->cmd.cmd, "1");

    char *args[] = {"program", "test", NULL};
    struct env *env = init_env(2, args);
    eval_redirection(&mom, &env);

    env_free(env);

    free(mom->children[2]->children[1]->cmd.cmd);
    free(mom->children[2]->cmd.cmd);
    free(mom->children[2]->children[1]);
    free(mom->children[2]->children[0]);
    free(mom->children[2]->children);
    free(mom->children[2]);

    free(mom->children[1]->children[1]->cmd.cmd);
    free(mom->children[1]->cmd.cmd);
    free(mom->children[1]->children[1]);
    free(mom->children[1]->children[0]);
    free(mom->children[1]->children);
    free(mom->children[1]);

    //free(mom->children[0]->cmd.cmd_arr[1]);
    free(mom->children[0]->cmd.cmd_arr[0]);
    free(mom->children[0]->cmd.cmd_arr);
    free(mom->children[0]);

    free(mom->children);
    free(mom);

    return 0;
}
*/
