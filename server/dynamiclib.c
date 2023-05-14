/*						Dynamic Library (send + receive)
 * 
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAXLINE 1024

extern void parse_cmd(int clientfd, char *arg);
//extern char* decrypt(char *encryption, char* original);
//extern char* encrypt (char* original, char* encryption);


//----------------------------------------FUNCTION DEFINITIONS--------------------------------------

/*int receive_cmd (int clientfd, char *cmd)
{
	// decrypt(cmd,cmd)
	int rc = recv(clientfd, cmd, 100, 0);

	//receive the command from the client
	if (rc == 0)	{
		printf ("Connection closed\n");
		return rc;
	}
	return rc;
}


void send_reply(int clientfd, char *cmd)
{
	//encrypt(cmd,cmd);
	parse_cmd(clientfd, cmd);
}*/
