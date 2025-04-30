/* fclock-test.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#define MAX_CMDLINE		4096
#define MAX_ARGS		64

void parse_cmd(void);
int get_cmd(struct flock *fl);
void disp_flock(const struct flock *fl);
void exit_sys(const char *msg);

char g_cmd[MAX_CMDLINE];
int g_count;
char *g_args[MAX_ARGS];

int main(int argc, char *argv[])
{
	int fd;
	pid_t pid;
	char *str;
	struct flock fl;
	int fcntl_cmd;

	if (argc != 2) {
		fprintf(stderr, "wrong number of arguments!...\n");
		exit(EXIT_FAILURE);
	}

	pid = getpid();

	if ((fd = open(argv[1], O_RDWR)) == -1)
		exit_sys("open");

	for (;;) {
		printf("CSD (%ld)>", (long)pid), fflush(stdout);
		fgets(g_cmd, MAX_CMDLINE, stdin);
		if ((str = strchr(g_cmd, '\n')) != NULL)
			*str = '\0';
		parse_cmd();
		if (g_count == 0)
			continue;
		if (g_count == 1 && !strcmp(g_args[0], "quit"))
			break;
		if (g_count != 4) {
			printf("invalid command!\n");
			continue;
		}

		if ((fcntl_cmd = get_cmd(&fl)) == -1) {
			printf("invalid command!\n");
			continue;
		}

		if (fcntl(fd, fcntl_cmd, &fl) == -1)
			if (errno == EACCES || errno == EAGAIN)
				printf("Locked failed!...\n");
			else
				perror("fcntl");
		if (fcntl_cmd == F_GETLK)
			disp_flock(&fl);
	}

	close(fd);

	return 0;
}

void parse_cmd(void)
{
	char *str;

	g_count = 0;
	for (str = strtok(g_cmd, " \t"); str != NULL; str = strtok(NULL, " \t"))
		g_args[g_count++] = str;
}

int get_cmd(struct flock *fl)
{
	int cmd, type;

	if (!strcmp(g_args[0], "F_SETLK"))
		cmd = F_SETLK;
	else if (!strcmp(g_args[0], "F_SETLKW"))
		cmd = F_SETLKW;
	else if (!strcmp(g_args[0], "F_GETLK"))
		cmd = F_GETLK;
	else
		return -1;

	if (!strcmp(g_args[1], "F_RDLCK"))
		type = F_RDLCK;
	else if (!strcmp(g_args[1], "F_WRLCK"))
		type = F_WRLCK;
	else if (!strcmp(g_args[1], "F_UNLCK"))
		type = F_UNLCK;
	else
		return -1;

	fl->l_type = type;
	fl->l_whence = SEEK_SET;
	fl->l_start = (off_t)strtol(g_args[2], NULL, 10);
	fl->l_len = (off_t)strtol(g_args[3], NULL, 10);

	return cmd;
}

void disp_flock(const struct flock *fl)
{
	switch (fl->l_type) {
		case F_RDLCK:
			printf("Read Lock\n");
			break;
		case F_WRLCK:
			printf("Write Lock\n");
			break;
		case F_UNLCK:
			printf("Unlocked (can be locked)\n");
	}

	printf("Whence: %d\n", fl->l_whence);
	printf("Start: %ld\n", (long)fl->l_start);
	printf("Length: %ld\n", (long)fl->l_len);
	if (fl->l_type != F_UNLCK)
		printf("Process Id: %ld\n", (long)fl->l_pid);
}

void exit_sys(const char *msg)
{
	perror(msg);

	exit(EXIT_FAILURE);
}
