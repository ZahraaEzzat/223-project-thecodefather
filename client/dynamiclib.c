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

//extern char* decrypt(char *encryption, char* original);
//extern char* encrypt (char* original, char* encryption);


//----------------------------------------FUNCTION DEFINITIONS--------------------------------------

int send_cmd(int fd, char *cmd) 
{
printf("send_cmd\n");
	int len = strlen(cmd);
//	encrypt(cmd, cmd);
	int rc = send(fd, cmd, len, 0);
	if (rc == -1 || rc == 0) {
		perror("send");
		return rc;
	}
	printf("send_cmd\n");
	return rc;
}


void receive_reply(int fd, char* buf)
{
printf("receive_reply\n");
	int rc = recv(fd, buf, MAXLINE, 0);

	if (rc > 0) {
		//decrypt(buf, buf);

		//write the received reply from the server
		if(write(1, buf, rc) < 0) {
			printf("%s", buf);
			exit(1);
		}
		if(rc == 0) {
			printf("end\n");
			return ;
		}
	}
	
	if (rc == -1) {
		printf("error in recv");
		exit(1);
	}
printf("receive_reply\n");
}
