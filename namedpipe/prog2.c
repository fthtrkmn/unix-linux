/* prog2.c */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFFER_SIZE		4096

void exit_sys(const char *msg);

int main(void)
{
	int fdpipe;
	ssize_t result;
	char buf[BUFFER_SIZE + 1];

	if ((fdpipe = open("mypipe", O_RDONLY)) == -1)
		exit_sys("open");

	while ((result = read(fdpipe, buf, BUFFER_SIZE)) > 0) {
		buf[result] = '\0';
		puts(buf);
	}
	if (result == -1)
		exit_sys("read");

	close(fdpipe);

	return 0;
}

void exit_sys(const char *msg)
{
	perror(msg);

	exit(EXIT_FAILURE);
}
