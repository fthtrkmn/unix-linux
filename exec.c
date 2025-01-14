#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <sys/syscall.h>   /* For SYS_xxx definitions */
#include <sys/wait.h>
#include <dirent.h>

#define MAX_ARG 4096
#define MAX_PATHS 100

int myexecl(const char *path, const char *arg0, ... /*, (char *)0 */);
int myexecle(const char *path, const char *arg0, ... /*, (char *)0, char *const envp[]*/);
int myexeclp(const char *file, const char *arg0, ... /*, (char *)0 */);
int myexecv(const char *path, char *const argv[]);
int myexecvp(const char *file, char *const argv[]);
void exit_sys(const char *msg);
char **parse_cmd_line(char *cmdline);

extern char **environ;
char *paths[MAX_PATHS];

int main(int argc, char *argv[])
{
	pid_t pid;
	const char *myenv[] = {"city=bursa","team=besiktas","company=ermaksan",NULL};
	const char *myargv[] = {"-l",NULL};
	if (argc == 1) {
		fprintf(stderr, "wrong number of arguments!...\n");
		exit(EXIT_FAILURE);
	}

	printf("sample running...\n");

	if ((pid = fork()) == -1)
		exit_sys("fork");

	/*if (pid == 0 && myexecle("mample", "mample", "ali", "veli", "selami", (char *)0,myenv) == -1)
		exit_sys("myexecle");

	if (pid == 0 && myexecv("mample", myargv) == -1)
		exit_sys("myexecv");

	if (pid == 0 && myexecl("mample", "mample", "ali", "veli", "selami", (char *)0) == -1)
		exit_sys("myexecl");
	*/
	/*if (pid == 0 && myexecvp("ls", (char * const *)myargv) == -1)
		exit_sys("myexecvp");*/
    
    if (pid == 0 && myexeclp("chown","chown","--help", (char *)0,myenv) == -1)
		exit_sys("myexeclp");

	printf("ok, parent continues...\n");

	if (waitpid(pid, NULL, 0) == -1)
		exit_sys("waitpid");

	return 0;
}

int myexeclp(const char *file, const char *arg0, ... /*, (char *)0 */)
{
    va_list vl;
    char *args[MAX_ARG + 1];
    char *arg;
    int i;
	char **env;
    va_start(vl,arg0);

    args[0] = (char *)arg0;
    for(i = 1; (arg = va_arg(vl,char *)) != NULL && i<MAX_ARG; ++i)
        args[i] = arg;
	env = (char**)va_arg(vl,char *);	
	va_end(vl);

    myexecvp(file,args);


}

int myexecle(const char *path, const char *arg0, ... /*, (char *)0, char *const envp[]*/)
{
    va_list vl;
    char *args[MAX_ARG + 1];
    char *arg;
    int i;
	char **env;
    va_start(vl,arg0);

    args[0] = (char *)arg0;
    for(i = 1; (arg = va_arg(vl,char *)) != NULL && i<MAX_ARG; ++i)
        args[i] = arg;
	env = (char**)va_arg(vl,char *);	
	va_end(vl);

	return syscall(SYS_execve,path, args, env);

}
int myexecv(const char *path, char *const argv[])
{
    
	return syscall(SYS_execve,path, argv, environ);


}
int myexecl(const char *path, const char *arg0, ... /*, (char *)0 */)
{
	va_list vl;
	char *args[MAX_ARG + 1];
	char *arg;
	int i;

	va_start(vl, arg0);

	args[0] = (char *)arg0;
	for (i = 1; (arg = va_arg(vl, char *)) != NULL && i < MAX_ARG; ++i)
		args[i] = arg;
	args[i] = NULL;

	va_end(vl);

	return syscall(SYS_execve,path, args, environ);

}

int myexecvp(const char *file, char *const argv[])
{
	if(strchr(file,'/') != NULL)
		myexecv(file,argv);
            
    char buffer[1024];
	char *value;
	char **paths;
	DIR *dir;
	struct dirent *dp;


	value = getenv("PATH");


	paths = parse_cmd_line(value);

	for(int i = 0; paths[i] != NULL; i++){
		if ((dir = opendir(paths[i] )) == NULL) {
        	exit_sys("opendir");
        	return -1;
    }
        do {
        if ((dp = readdir(dir)) != NULL) {
            if (strcmp(dp->d_name, file) != 0)
                continue;
            closedir(dir);
            sprintf(buffer,"%s/%s",paths[i],file);
    		myexecv(buffer,argv);
            return 0;
 

        }
    } while (dp != NULL);

	}

	return -1;

}
char **parse_cmd_line(char *cmdline)
{
	char *str;
	int g_nparams = 0;
	
	for (str = strtok(cmdline, ":"); str != NULL; str = strtok(NULL, ":"))
		paths[g_nparams++] = str;
	paths[g_nparams] = NULL;
	return paths;
}

void exit_sys(const char *msg)
{
	perror(msg);

	exit(EXIT_FAILURE);
}
