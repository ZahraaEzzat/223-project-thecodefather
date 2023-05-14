/* 
 *
 * Implementation of 'parse_cmd' shell command 
 *
 * 'parse_cmd' will take the command given by user input and resolve it.
 * A function (that implements the command) will be called to resolve the corresponding command.
 * This code should be statically linked with server.c (run "make")
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>


extern void _exitt(int);
extern void help(int);
extern void ls_cmd(int clientfd, char *dir_name);
extern void cat(int clientfd, char *filename);
extern void ret(int clientfd, char *file_name);
char *cmdn;

void parse_cmd(int clientfd, char *cmd)
{
	printf("Parse: %s\n", cmd);
	cmd[strlen(cmd)]='\0';
	char *arg = strtok(cmd, " ");
	
	cmdn = (char *)malloc(sizeof(char)*100);
        strcpy(cmdn, arg);

	if ((strcmp(cmdn, "help")) == 0)
		help(clientfd);

	else if (strcmp(cmdn, "exit") == 0)
		_exitt(clientfd);

	else if ((strcmp(cmdn , "cat") == 0) || (strcmp(arg , "ls")) == 0 || (strcmp(arg , "ret")) == 0) {
		arg = strtok(NULL, " ");

		if ((strcmp(cmdn, "cat") == 0) && arg == NULL)
			cat(clientfd, NULL);

		else if ((strcmp(cmdn, "ls") == 0) && arg == NULL)
			ls_cmd(clientfd, NULL);

		else if (strcmp(cmdn, "cat") == 0) {
			while (arg != NULL) {
				cat(clientfd, arg);
				arg = strtok(NULL, " ");
			}
		}

		else if (strcmp(cmdn, "ls") == 0)
			ls_cmd(clientfd, arg);

		else if (strcmp(cmdn, "ret") == 0)
			ret(clientfd, arg);
	}

	else { 
		send(clientfd,"Not valid\n", strlen("Not valid\n"), 0);
	}
}
