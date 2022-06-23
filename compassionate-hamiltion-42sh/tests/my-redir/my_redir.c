#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>


int redirection(char *filename, char *argv[])
{
	// duplication
	int stdoutDup = dup(STDOUT_FILENO);

	int file1_fd = open(filename,  O_CREAT | O_TRUNC | O_WRONLY, 0755);
	if (file1_fd < 0)
		errx(1, "ERROR: failure in opening the file.");

	if (dup2(file1_fd, STDOUT_FILENO) == -1)
		errx(1, "ERROR: failure in dup2().");
	
	// forking
    int childPid = fork();
    if (childPid == -1)
        errx(1, "ERROR: failure in forking.");

    // child process: execute commands
    else if (childPid == 0)
    {
        if (execvp(argv[0], argv) == -1) // argv[1], argv + 1
        	errx(127, "ERROR: failure in execvp() in child process.");
    }
    // parent process
    else
    {
        int childStatus = 0;
        if (waitpid(childPid, &childStatus, 0) == -1)
            errx(1, "ERROR: failure in execvp() in parent process.");
    }

    fflush(stdout);

    dup2(stdoutDup, STDOUT_FILENO);
    close(stdoutDup);

    return 0;
}

int main(int argc, char *argv[])
{
	if (argc < 3)
		errx(1, "ERROR: too few arguments.");

	printf("%d\n", redirection(argv[1], argv + 2));

	return 0;
}