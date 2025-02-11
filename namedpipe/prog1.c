/* prog1.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFFER_SIZE		4096

void exit_sys(const char *msg);

int main(void)
{
	int fdpipe;
	char buf[BUFFER_SIZE];
	char *str;

	if ((fdpipe = open("mypipe", O_WRONLY)) == -1)
		exit_sys("open");

	for (;;) {
		printf("Text:");
		fflush(stdout);
		if (fgets(buf, BUFFER_SIZE, stdin) != NULL)
			if ((str = strchr(buf, '\n')) != NULL)
				*str = '\0';
		*str = '\0';
		if (*buf == '\0')
			continue;
		if (write(fdpipe, buf, strlen(buf)) == -1)
			exit_sys("write");
		if (!strcmp(buf, "quit"))
			break;
	}

	close(fdpipe);

	return 0;
}

void exit_sys(const char *msg)
{
	perror(msg);

	exit(EXIT_FAILURE);
}
