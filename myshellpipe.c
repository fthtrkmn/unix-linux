#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_CMD_LINE        4096
#define MAX_CMD_PARAMS      128

typedef struct tagCMD {
    char *name;
    void (*proc)(void);
} CMD;

void parse_cmd_line(char *cmdline);
void cd_proc(void);
void rv_proc(void);
void exit_sys(const char *msg);
void exit_sys_child(const char *msg);
char *strrev(char *str);


char *g_params[MAX_CMD_PARAMS];
int g_nparams;
char g_cwd[PATH_MAX];

CMD g_cmds[] = {
    {"cd", cd_proc},
    {"rv", rv_proc},//reverse text
    {NULL, NULL}
};

int main(void)
{
    char cmdline[MAX_CMD_LINE];
    char *str;
    int i;
    pid_t pid;

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
        if (g_cmds[i].name == NULL) {
            if ((pid = fork()) == -1) {
                perror("fork");
                continue;
            }
            if (pid == 0 && execvp(g_params[0], &g_params[0]) == -1){
                fprintf(stderr, "command not found or cannot execute!\n");
                continue;
            }
            if (wait(NULL) == -1)
                exit_sys("wait");
        }
    }

    return 0;
}

void parse_cmd_line(char *cmdline)
{
    char *str;

    g_nparams = 0;
    for (str = strtok(cmdline, " \t"); str != NULL; str = strtok(NULL, " \t"))
        g_params[g_nparams++] = str;
    g_params[g_nparams] = NULL;
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

void rv_proc(void)
{
    int pipefds_1[2], pipefds_2[2];
    pid_t pid;
    char text[1024];
    
    int fds;
    if (g_nparams > 2) {
        printf("rv: too many arguments!\n");
        return;
    }

    if(pipe(pipefds_1) == -1)
        exit_sys("pipe");

    if(pipe(pipefds_2) == -1)
        exit_sys("pipe");

    if((pid = fork()) == -1)
        exit_sys("fork");

    for(;;){
        if(pid != 0) {
            close(pipefds_1[0]);
            close(pipefds_2[1]);

            fgets(text,1024,stdin);
            if(write(pipefds_1[1],text,(sizeof text)) == -1)
            exit_sys("write");
            if(read(pipefds_2[0],text,(sizeof text)) == -1)
                exit_sys("read");

            printf("%s",text);
            printf("\n");

        }
    else{                    
            close(pipefds_2[0]);
            close(pipefds_1[1]);
            if(read(pipefds_1[0],text,(sizeof text)) == -1)
                exit_sys("read");

            if(write(pipefds_2[1],strrev(text),(sizeof text)) == -1)
                exit_sys("write");   
        }

    }


    close(pipefds_1[0]);
	close(pipefds_2[0]);
    close(pipefds_1[1]);
	close(pipefds_2[1]);

	if (wait(NULL) == -1)
		exit_sys("wait");

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
char *strrev(char *str)
{
    if (!str || ! *str)
        return str;

    int i = strlen(str) - 1, j = 0;

    char ch;
    while (i > j)
    {
        ch = str[i];
        str[i] = str[j];
        str[j] = ch;
        i--;
        j++;
    }
    str[j-1] = NULL;
    return str;
}
