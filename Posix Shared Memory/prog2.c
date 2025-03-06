/* prog2.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>

#define SHM_NAME		"/sample_posix_shared_memory"
#define SHM_SIZE		4096

void exit_sys(const char *msg);

int main(void)
{
	int fdshm;
	char *shmaddr;

	if ((fdshm = shm_open(SHM_NAME, O_RDWR, 0)) == -1)
		exit_sys("shm_open");

	shmaddr = (char *)mmap(NULL, SHM_SIZE, PROT_WRITE, MAP_SHARED, fdshm, 0);
	if (shmaddr == MAP_FAILED)
		exit_sys("mmap");

	for (;;) {
		printf("Press ENTER to read...");
		getchar();
		puts(shmaddr);
		if (!strcmp(shmaddr, "quit"))
			break;
	}

	if (munmap(shmaddr, SHM_SIZE) == -1)
		exit_sys("munmap");

	close(fdshm);

	return 0;
}

void exit_sys(const char *msg)
{
	perror(msg);

	exit(EXIT_FAILURE);
}
