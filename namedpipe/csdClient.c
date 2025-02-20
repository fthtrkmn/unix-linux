/* server.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

/* Symbolic Constants */

#define SERVER_PIPE		"serverpipe"
#define MAX_CMD_LEN		1024
#define MAX_MSG_LEN		32768
#define MAX_PIPE_PATH	1024

/* Type Declaration */

typedef struct tagCLIENT_MSG {
	int msglen;
	int client_id;
	char msg[MAX_MSG_LEN];
} CLIENT_MSG;

typedef struct tagSERVER_MSG {
	int msglen;
	char msg[MAX_MSG_LEN];
} SERVER_MSG;

typedef struct tagMSG_CONTENTS {
	char *msg_cmd;
	char *msg_param;
} MSG_CONTENTS;

typedef struct tagMSG_PROC {
	const char *msg_cmd;
	int (*proc)(const char *msg_param);
} MSG_PROC;

/* Function Prototypes */

void sigpipe_handler(int sno);
int putmsg(const char *cmd);
int get_server_msg(int fdp, SERVER_MSG *smsg);
void parse_msg(char *msg, MSG_CONTENTS *msgc);
void check_quit(char *cmd);
int connect_to_server(void);
int cmd_response_proc(const char *msg_param);
int disconnect_accepted_proc(const char *msg_param);
int invalid_command_proc(const char *msg_param);
void clear_stdin(void);
void exit_sys(const char *msg);

/* Global Data Definitions */

MSG_PROC g_msg_proc[] = {
	{"CMD_RESPONSE", cmd_response_proc},
	{"DISCONNECT_ACCEPTED", disconnect_accepted_proc},
	{"INVALID_COMMAND", invalid_command_proc},
	{NULL, NULL}
};

int g_client_id;
int g_fdps, g_fdpc;

/* Function Definitions */

int main(void)
{
	char cmd[MAX_CMD_LEN];
	char *str;
	SERVER_MSG smsg;
	MSG_CONTENTS msgc;
	int i;

	if (signal(SIGPIPE, sigpipe_handler) == SIG_ERR)
		exit_sys("signal");

	if ((g_fdps = open(SERVER_PIPE, O_WRONLY)) == -1)
		exit_sys("open");

	if (connect_to_server() == -1) {
		fprintf(stderr, "cannot connect to server! Try again...\n");
		exit(EXIT_FAILURE);
	}

	for (;;) {
		printf("Client>");
		fflush(stdout);
		fgets(cmd, MAX_CMD_LEN, stdin);
		if ((str = strchr(cmd, '\n')) != NULL)
			*str = '\0';

		check_quit(cmd);

		if (putmsg(cmd) == -1)
			exit_sys("putmsg");

		if (get_server_msg(g_fdpc, &smsg) == -1)
			exit_sys("get_client_msg");

		parse_msg(smsg.msg, &msgc);

		for (i = 0; g_msg_proc[i].msg_cmd != NULL; ++i)
			if (!strcmp(msgc.msg_cmd, g_msg_proc[i].msg_cmd)) {
				if (g_msg_proc[i].proc(msgc.msg_param) == -1) {
					fprintf(stderr, "command failed!\n");
					exit(EXIT_FAILURE);
				}
				break;
			}
		if (g_msg_proc[i].msg_cmd == NULL) {		/* command not found */
			fprintf(stderr, "Fatal Error: Unknown server message!\n");
			exit(EXIT_FAILURE);
		}
	}

	return 0;
}

void sigpipe_handler(int sno)
{
	printf("server down, exiting...\n");

	exit(EXIT_FAILURE);
}

int putmsg(const char *cmd)
{
	CLIENT_MSG cmsg;
	int i, k;

	for (i = 0; isspace(cmd[i]); ++i)
		;
	for (k = 0; !isspace(cmd[i]); ++i)
		cmsg.msg[k++] = cmd[i];
	cmsg.msg[k++] = ' ';
	for (; isspace(cmd[i]); ++i)
		;
	for (; (cmsg.msg[k++] = cmd[i]) != '\0'; ++i)
		;
	cmsg.msglen = (int)strlen(cmsg.msg);
	cmsg.client_id = g_client_id;

	if (write(g_fdps, &cmsg, 2 * sizeof(int) + cmsg.msglen) == -1)
		return -1;

	return 0;
}

int get_server_msg(int fdp, SERVER_MSG *smsg)
{
	if (read(fdp, &smsg->msglen, sizeof(int)) == -1)
		return -1;

	if (read(fdp, smsg->msg, smsg->msglen) == -1)
		return -1;

	smsg->msg[smsg->msglen] = '\0';

	return 0;
}

void parse_msg(char *msg, MSG_CONTENTS *msgc)
{
	int i;

	msgc->msg_cmd = msg;

	for (i = 0; msg[i] != ' ' && msg[i] != '\0'; ++i)
		;
	msg[i++] = '\0';
	msgc->msg_param = &msg[i];
}

void check_quit(char *cmd)
{
	int i, pos;

	for (i = 0; isspace(cmd[i]); ++i)
		;
	pos = i;
	for (; !isspace(cmd[i]) && cmd[i] != '\0'; ++i)
		;
	if (!strncmp(&cmd[pos], "quit", pos - i))
		strcpy(cmd, "DISCONNECT_REQUEST");
}

int connect_to_server(void)
{
	char name[MAX_PIPE_PATH];
	char cmd[MAX_CMD_LEN];
	char *str;
	SERVER_MSG smsg;
	MSG_CONTENTS msgc;
	int response;

	printf("Pipe name:");
	fgets(name, MAX_PIPE_PATH, stdin);
	if ((str = strchr(name, '\n')) != NULL)
		*str = '\0';

	if (access(name, F_OK) == 0) {
		do {
			printf("Pipe already exists! Overwrite? (Y/N)");
			fflush(stdout);
			response = tolower(getchar());
			clear_stdin();
			if (response == 'y' && remove(name) == -1)
				return -1;
		} while (response != 'y' && response != 'n');
		if (response == 'n')
			return -1;
	}

	sprintf(cmd, "CONNECT %s", name);

	if (putmsg(cmd) == -1)
		return -1;

	while (access(name, F_OK) != 0)
		usleep(300);

	if ((g_fdpc = open(name, O_RDONLY)) == -1)
		return -1;

	if (get_server_msg(g_fdpc, &smsg) == -1)
		exit_sys("get_client_msg");

	parse_msg(smsg.msg, &msgc);

	if (strcmp(msgc.msg_cmd, "CONNECTED"))
		return -1;

	g_client_id = (int)strtol(msgc.msg_param, NULL, 10);

	printf("Connected server with '%d' id...\n", g_client_id);

	return 0;
}

int cmd_response_proc(const char *msg_param)
{
	printf("%s\n", msg_param);

	return 0;
}

int disconnect_accepted_proc(const char *msg_param)
{
	if (putmsg("DISCONNECT") == -1)
			exit_sys("putmsg");

	exit(EXIT_SUCCESS);

	return 0;
}

int invalid_command_proc(const char *msg_param)
{
	printf("invalid command: %s\n", msg_param);

	return 0;
}

void clear_stdin(void)
{
	int ch;

	while ((ch = getchar()) != '\n' && ch != EOF)
		;
}

void exit_sys(const char *msg)
{
	perror(msg);

	exit(EXIT_FAILURE);
}
