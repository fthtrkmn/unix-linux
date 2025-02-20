/* client.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>

/* Symbolic Constants */

#define SERVER_PIPE			"serverpipe"
#define MAX_MSG_LEN			32768
#define MAX_PIPE_PATH		1024
#define MAX_CLIENT			1024

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
	int (*proc)(int, const char *msg_param);
} MSG_PROC;

typedef struct tagCLIENT_INFO {
	int fdp;
	char path[MAX_PIPE_PATH];
} CLIENT_INFO;

/* Function Prototypes */

int get_client_msg(int fdp, CLIENT_MSG *cmsg);
int putmsg(int client_id, const char *cmd);
void parse_msg(char *msg, MSG_CONTENTS *msgc);
void print_msg(const CLIENT_MSG *cmsg);
int invalid_command(int client_id, const char *cmd);
int connect_proc(int client_id, const char *msg_param);
int disconnect_request_proc(int client_id, const char *msg_param);
int disconnect_proc(int client_id, const char *msg_param);
int cmd_proc(int client_id, const char *msg_param);
void exit_sys(const char *msg);

/* Global Data Definitions */

MSG_PROC g_msg_proc[] = {
	{"CONNECT", connect_proc},
	{"DISCONNECT_REQUEST", disconnect_request_proc},
	{"DISCONNECT", disconnect_proc},
	{"CMD", cmd_proc},
	{NULL, NULL}
};

CLIENT_INFO g_clients[MAX_CLIENT];

/* Function Definitions */

int main(void)
{
	int fdp;
	CLIENT_MSG cmsg;
	MSG_CONTENTS msgc;
	int i;

	printf("Server running...\n");

	if ((fdp = open(SERVER_PIPE, O_RDWR)) == -1)
		exit_sys("open");

	for (;;) {
		if (get_client_msg(fdp, &cmsg) == -1)
			exit_sys("get_client_msg");
		print_msg(&cmsg);
		parse_msg(cmsg.msg, &msgc);
		for (i = 0; g_msg_proc[i].msg_cmd != NULL; ++i)
			if (!strcmp(msgc.msg_cmd, g_msg_proc[i].msg_cmd)) {
				if (g_msg_proc[i].proc(cmsg.client_id, msgc.msg_param)) {

				}
				break;
			}
		if (g_msg_proc[i].msg_cmd == NULL)
			if (invalid_command(cmsg.client_id, msgc.msg_cmd) == -1)
				continue;
	}

	close(fdp);

	return 0;
}

int get_client_msg(int fdp, CLIENT_MSG *cmsg)
{
	if (read(fdp, &cmsg->msglen, sizeof(int)) == -1)
		return -1;

	if (read(fdp, &cmsg->client_id, sizeof(int)) == -1)
		return -1;

	if (read(fdp, cmsg->msg, cmsg->msglen) == -1)
		return -1;

	cmsg->msg[cmsg->msglen] = '\0';

	return 0;
}

int putmsg(int client_id, const char *cmd)
{
	SERVER_MSG smsg;
	int fdp;

	strcpy(smsg.msg, cmd);
	smsg.msglen = strlen(smsg.msg);

	fdp = g_clients[client_id].fdp;

	return write(fdp, &smsg, sizeof(int) + smsg.msglen) == -1 ? -1 : 0;
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

void print_msg(const CLIENT_MSG *cmsg)
{
	printf("Message from \"%s\": %s\n", cmsg->client_id ? g_clients[cmsg->client_id].path : "", cmsg->msg);
}

int invalid_command(int client_id, const char *cmd)
{
	char buf[MAX_MSG_LEN];

	sprintf(buf, "INVALID_COMMAND %s", cmd);
	if (putmsg(client_id, buf) == -1)
		return -1;

	return 0;
}

int connect_proc(int client_id, const char *msg_param)
{
	int fdp;
	char buf[MAX_MSG_LEN];

	if (mkfifo(msg_param, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH) == -1) {
		printf("CONNECT message failed! Params = \"%s\"\n", msg_param);
		return -1;
	}

	if ((fdp = open(msg_param, O_WRONLY)) == -1)
		exit_sys("open");

	g_clients[fdp].fdp = fdp;
	strcpy(g_clients[fdp].path, msg_param);

	sprintf(buf, "CONNECTED %d", fdp);
	if (putmsg(fdp, buf) == -1)
		exit_sys("putmsg");

	return 0;
}

int disconnect_request_proc(int client_id, const char *msg_param)
{
	if (putmsg(client_id, "DISCONNECT_ACCEPTED") == -1)
		return -1;

	return 0;
}

int disconnect_proc(int client_id, const char *msg_param)
{
	close(g_clients[client_id].fdp);

	if (remove(g_clients[client_id].path) == -1)
		return -1;

	return 0;
}

int cmd_proc(int client_id, const char *msg_param)
{
	FILE *f;
	char cmd[MAX_MSG_LEN] = "CMD_RESPONSE ";
	int i;
	int ch;

	if ((f = popen(msg_param, "r")) == NULL) {
		printf("cannot execute shell command!...\n");
		return -1;
	}

	for (i = 13; (ch = fgetc(f)) != EOF; ++i)
		cmd[i] = ch;
	cmd[i] = '\0';

	if (putmsg(client_id, cmd) == -1)
		return -1;

	return 0;
}

void exit_sys(const char *msg)
{
	perror(msg);

	exit(EXIT_FAILURE);
}
