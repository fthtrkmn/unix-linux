#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <limits.h>
#include <unistd.h>

#define MAX_CMD_LINE		4096
#define MAX_CMD_PARAMS		128

typedef struct tagCMD {
	char *name;
	void (*proc)(void);
} CMD;

void parse_cmd_line(char *cmdline);

void dir_proc(void);
void clear_proc(void);
void pwd_proc(void);
void cd_proc(void);
void umask_proc(void);

int check_umask_arg(const char *str);

void exit_sys(const char *msg);

char *g_params[MAX_CMD_PARAMS];
int g_nparams;
char g_cwd[PATH_MAX];

CMD g_cmds[] = {
	{"dir", dir_proc},
	{"clear", clear_proc},
	{"pwd", pwd_proc},
	{"cd", cd_proc},
	{"umask", umask_proc},
	{NULL, NULL}
};

int main(void)
{
	char cmdline[MAX_CMD_LINE];
	char *str;
	int i;

	if (getcwd(g_cwd, PATH_MAX) == NULL)
		exit_sys("fatal error (getcwd)");

	for (;;) {
		printf("CSD:%s>", g_cwd);
		if (fgets(cmdline, MAX_CMD_LINE, stdin) == NULL)
			continue;
		if ((str = strchr(cmdline, '\n')) != NULL)
			*str = '\0';
		parse_cmd_line(cmdline);
		if (g_nparams == 0)
			continue;
		if (!strcmp(g_params[0], "exit"))
			break;
		for (i = 0; g_cmds[i].name != NULL; ++i)
			if (!strcmp(g_params[0], g_cmds[i].name)) {
				g_cmds[i].proc();
				break;
			}
		if (g_cmds[i].name == NULL)
			printf("bad command: %s\n", g_params[0]);
	}

	return 0;
}

void parse_cmd_line(char *cmdline)
{
	char *str;

	g_nparams = 0;
	for (str = strtok(cmdline, " \t"); str != NULL; str = strtok(NULL, " \t"))
		g_params[g_nparams++] = str;
}

void dir_proc(void)
{
	printf("dir command executing...\n");
}

void clear_proc(void)
{
	system("clear");
}

void pwd_proc(void)
{
	printf("%s\n", g_cwd);
}

void cd_proc(void)
{
	char *dir;

	if (g_nparams > 2) {
		printf("too many arguments!\n");
		return;
	}
	if (g_nparams == 1) {
		if ((dir = getenv("HOME")) == NULL)
			exit_sys("fatal error (getenv");
	}
	else
		dir = g_params[1];

	if (chdir(dir) == -1) {
		printf("%s\n", strerror(errno));
		return;
	}

	if (getcwd(g_cwd, PATH_MAX) == NULL)
		exit_sys("fatal error (getcwd)");
}

void umask_proc(void)
{
	mode_t mode;
	int argval;

	if (g_nparams > 2) {
		printf("too many arguments in umask command!...\n");
		return;
	}

	if (g_nparams == 1) {
		mode = umask(0);
		umask(mode);

		printf("%04o\n", (int)mode);

		return;
	}

	if (!check_umask_arg(g_params[1])) {
		printf("%s octal number out of range!...\n", g_params[1]);
		return;
	}

	sscanf(g_params[1], "%o", &argval);
	umask(argval);
}

int check_umask_arg(const char *str)
{
	if (strlen(str) > 4)
		return 0;

	for (int i = 0; str[i] != '\0'; ++i)
		if (str[i] < '0' || str[i] > '7')
			return 0;

	return 1;
}

void exit_sys(const char *msg)
{
	perror(msg);

	exit(EXIT_FAILURE);
}
