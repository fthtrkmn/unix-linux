/* proc2.c */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFFER_SIZE		4096

void exit_sys(const char *msg);

int main(void)
{
	int fdpipe;
	ssize_t result;
	int val;

	if ((fdpipe = open("mypipe", O_RDONLY)) == -1)
		exit_sys("open");

	if (fcntl(fdpipe, F_SETFL, fcntl(fdpipe, F_GETFL) | O_NONBLOCK) == -1)
		exit_sys("fcntl");

	while ((result = read(fdpipe, &val, sizeof(int))) != 0) {
			if (result == -1)
				if (errno == EAGAIN) {
					printf("child background processing...\n");
					usleep(500);
					continue;
				}
				else
					exit_sys("read");
			printf("%d\n", val);
		}

	close(fdpipe);

	return 0;
}

void exit_sys(const char *msg)
{
	perror(msg);

	exit(EXIT_FAILURE);
}
