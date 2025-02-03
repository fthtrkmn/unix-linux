#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_ARG		1024

void exit_sys(const char *msg);
void exit_sys_child(const char *msg);

int main(int argc, char *argv[])
{
	char *ppos, *str;
	char *cmdl[MAX_ARG + 1], *cmdr[MAX_ARG + 1];
	int pipefds[2];
	pid_t pidl, pidr;
	int n;

	if (argc != 2) {
		fprintf(stderr, "wrong number of arguments!...\n");
		exit(EXIT_FAILURE);
	}

	if ((ppos = strchr(argv[1], '|')) == NULL) {
		fprintf(stderr, "invalid argument: %s\n", argv[1]);
		exit(EXIT_FAILURE);
	}
	*ppos = '\0';

	n = 0;
	for (str = strtok(argv[1], " \t"); str != NULL; str = strtok(NULL, " \t"))
		cmdl[n++] = str;
	cmdl[n] = NULL;

	if (n == 0) {
		fprintf(stderr, "invalid argument!...\n");
		exit(EXIT_FAILURE);
	}

	n = 0;
	for (str = strtok(ppos + 1, " \t"); str != NULL; str = strtok(NULL, " \t"))
		cmdr[n++] = str;
	cmdr[n] = NULL;

	if (n == 0) {
		fprintf(stderr, "invalid argument!...\n");
		exit(EXIT_FAILURE);
	}

	if (pipe(pipefds) == -1)
		exit_sys("pipe");

	if ((pidl = fork()) == -1)
		exit_sys("fork");

	if (pidl == 0) {
		close(pipefds[0]);
		if (pipefds[1] != 1) {				/* bu kontrol normal durumda yapılmayabilir */
			if (dup2(pipefds[1], 1) == -1)
				exit_sys_child("dup2");
			close(pipefds[1]);
		}

		if (execvp(cmdl[0], cmdl) == -1)
			exit_sys_child("execvp");

		/* unreachable code*/
	}

	if ((pidr = fork()) == -1)
		exit_sys("fork");

	if (pidr == 0) {
		close(pipefds[1]);
		if (pipefds[0] != 0) {				/* bu kontrol normal durumda yapılmayabilir */
			if (dup2(pipefds[0], 0) == -1)
				exit_sys_child("dup2");
			close(pipefds[0]);
		}

		if (execvp(cmdr[0], cmdr) == -1)
			exit_sys_child("execvp");

		/* unreachable code*/
	}

	close(pipefds[0]);
	close(pipefds[1]);

	if (waitpid(pidl, NULL, 0) == -1)
		exit_sys("waitpid");

	if (waitpid(pidr, NULL, 0) == -1)
		exit_sys("waitpid");

	return 0;
}

void exit_sys(const char *msg)
{
	perror(msg);

	exit(EXIT_FAILURE);
}

void exit_sys_child(const char *msg)
{
	perror(msg);

	_exit(EXIT_FAILURE);
}
