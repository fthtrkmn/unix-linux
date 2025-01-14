#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pwd.h>

void exit_sys(const char *msg);

int main(int argc, char *argv[])
{
	struct passwd *pass;

	if (argc != 2) {
		fprintf(stderr, "wrong number of arguments!...\n");
		exit(EXIT_FAILURE);
	}

	errno = 0;
	if ((pass = getpwnam(argv[1])) == NULL) {
		if (errno == 0) {
			fprintf(stderr, "user name cannot found!...\n");
			exit(EXIT_FAILURE);
		}
		exit_sys("getpwnam");
	}

	printf("User Name: %s\n", pass->pw_name);
	printf("Password: %s\n", pass->pw_passwd);
	printf("User id: %llu\n", (unsigned long long)pass->pw_uid);
	printf("Group id: %llu\n", (unsigned long long)pass->pw_gid);
	printf("Gecos: %s\n", pass->pw_gecos);
	printf("Current Working Directory: %s\n", pass->pw_dir);
	printf("Login Program: %s\n", pass->pw_shell);

	return 0;
}

void exit_sys(const char *msg)
{
	perror(msg);

	exit(EXIT_FAILURE);
}
