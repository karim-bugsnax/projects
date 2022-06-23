#include <err.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int exec_pipe(char **argv_left, char **argv_right)
{
    int fds[2]; // 0 = read, 1 = write
    if (pipe(fds) == -1)
        errx(1, "Failed to create pipe file descriptors.");

    int pid = fork();
    if (pid == -1)
        errx(1, "Could not fork\n");
    else if (pid == 0) // Child (in which we fork again)
    {
        int pid2 = fork();
        if (pid2 == -1)
            errx(1, "Could not fork\n");

        else if (pid2 == 0) // Child 1 (handle argv_left)
        {
            close(fds[0]);
            if (dup2(fds[1], STDOUT_FILENO) == -1)
                errx(1, "Dup2 error.");

            if (execvp(argv_left[0], argv_left) == -1)
                errx(127, "Exevp could not be executed.");
        }
        else // Child 2 (handle argv_right)
        {
            close(fds[1]);
            if (dup2(fds[0], STDIN_FILENO) == -1)
                errx(1, "Dup2 error.");

            int wstatus;
            if (waitpid(pid2, &wstatus, 0) == -1)
                errx(1, "Error, no args or failed execvp\n");

            if (execvp(argv_right[0], argv_right) == -1)
                errx(127, "Exevp could not be executed.");
        }
    }
    else // Main Parent
    {
        close(fds[0]);
        close(fds[1]);

        int wstatus;
        if (waitpid(pid, &wstatus, 0) == -1)
            errx(1, "Error, no args or failed execvp\n");
        //printf("process exit status: %d\n", WEXITSTATUS(wstatus));
    }
    return 0;
}

#include <stdio.h>
int main(void)
{
    char *argv_left[3] = {"echo", "Hallo", NULL};
    char *argv_right[4] = {"tr", "a", "e", NULL};
    printf("%d\n", exec_pipe(argv_left, argv_right));
}