
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

void exit_sys(const char *msg);
void exit_sys_child(const char *msg);

int main(int argc, char *argv[])
{
	int pipefds[2];
	pid_t pid;
	ssize_t result;
	int val;
	int i;

	if (pipe(pipefds) == -1)
		exit_sys("pipe");

	if (fcntl(pipefds[1], F_SETFL, fcntl(pipefds[1], F_GETFL) | O_NONBLOCK) == -1)
		exit_sys("fcntl");

	if (fcntl(pipefds[0], F_SETFL, fcntl(pipefds[0], F_GETFL) | O_NONBLOCK) == -1)
		exit_sys("fcntl");

	if ((pid = fork()) == -1)
		exit_sys("fork");

	if (pid != 0) {		/* parent writes */
		close(pipefds[0]);
		sleep(1);

		i = 0;
		while (i < 100000) {
			if (write(pipefds[1], &i, sizeof(int)) == -1)
				if (errno == EAGAIN) {
					printf("parent background processing...\n");
					usleep(500);
					continue;
				}
				else
					exit_sys("write");
			++i;
		}

		close(pipefds[1]);
		if (wait(NULL) == -1)
			exit_sys("wait");

	}
	else {			/* child reads */
		close(pipefds[1]);
		while ((result = read(pipefds[0], &val, sizeof(int))) != 0) {
			if (result == -1)
				if (errno == EAGAIN) {
					printf("child background processing...\n");
					usleep(500);
					continue;
				}
				else
					exit_sys_child("read");
			printf("%d\n", val);
		}

		close(pipefds[0]);
	}

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
