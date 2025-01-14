#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#define BUFFER_SIZE		4096

void exit_sys(const char *msg);

int main(int argc, char *argv[])
{
	char buf[BUFFER_SIZE];
	int fds, fdd;
	ssize_t result;

	if (argc != 3) {
		fprintf(stderr, "wrong number of arguments!...\n");
		exit(EXIT_FAILURE);
	}

	if ((fds = open(argv[1], O_RDONLY)) == -1)
		exit_sys(argv[1]);

        
	if ((fdd = open(argv[2], O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)) == -1)
		exit_sys(argv[2]);

	while ((result = read(fds, buf, BUFFER_SIZE)) > 0)
		if (write(fdd, buf, result) != result) {
			fprintf(stderr, "cannot write file!...\n");
			exit(EXIT_FAILURE);
		}

	if (result == -1)
		exit_sys("read");

	close(fds);
	close(fdd);

	return 0;
}

void exit_sys(const char *msg)
{
	perror(msg);

	exit(EXIT_FAILURE);
}
