/* proc1.c */

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
	int i;

	if ((fdpipe = open("mypipe", O_WRONLY)) == -1)
		exit_sys("open");

	if (fcntl(fdpipe, F_SETFL, fcntl(fdpipe, F_GETFL) | O_NONBLOCK) == -1)
		exit_sys("fcntl");

	i = 0;
	while (i < 100000) {
		if (write(fdpipe, &i, sizeof(int)) == -1)
			if (errno == EAGAIN) {
				printf("parent background processing...\n");
				usleep(500);
				continue;
			}
			else
				exit_sys("write");
		++i;
	}

	close(fdpipe);

	return 0;
}

void exit_sys(const char *msg)
{
	perror(msg);

	exit(EXIT_FAILURE);
}
