/* prog1.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>

#define SHM_NAME		"/sample_posix_shared_memory"
#define SHM_SIZE		4096

void exit_sys(const char *msg);

int main(void)
{
	int fdshm;
	char *shmaddr;
	char buf[4096];
	char *str;

	if ((fdshm = shm_open(SHM_NAME, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)) == -1)
		exit_sys("shm_open");

	if (ftruncate(fdshm, SHM_SIZE) == -1)
		exit_sys("ftruncate");

	shmaddr = (char *)mmap(NULL, SHM_SIZE, PROT_WRITE, MAP_SHARED, fdshm, 0);
	if (shmaddr == MAP_FAILED)
		exit_sys("mmap");

	for (;;) {
		printf("Text:");

		/* okuma doğrudan paylaşılan bellek alanına da yapılabilir */

		if (fgets(buf, 4096, stdin) == NULL)
			continue;
		if ((str = strchr(buf, '\n')) != NULL)
			*str = '\0';
		strcpy(shmaddr, buf);
		if (!strcmp(buf, "quit"))
			break;
	}

	if (munmap(shmaddr, SHM_SIZE) == -1)
		exit_sys("munmap");

	close(fdshm);

	if (shm_unlink(SHM_NAME) == -1)
		exit_sys("shm_unlink");

	return 0;
}

void exit_sys(const char *msg)
{
	perror(msg);

	exit(EXIT_FAILURE);
}
