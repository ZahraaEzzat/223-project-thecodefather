/*						Dynamic Library (send + receive)
 * 
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>

#define MAXLINE 6144

//extern char* decrypt(char *encryption, char* original);
//extern char* encrypt (char* original, char* encryption);


//----------------------------------------FUNCTION DEFINITIONS--------------------------------------

/*int send_cmd(int fd, char *cmd) 
{
	int len = strlen(cmd);
//	encrypt(cmd, cmd);
	int rc = send(fd, cmd, len, 0);
	if (rc == -1 || rc == 0) {
		perror("send");
		return rc;
	}
	return rc;
}


void receive_reply(int clientfd, char* buf)
{
	int rc;
	if (strstr(buf, "ret") != NULL) {
		//receive the size of the file
		int rsize = recv(clientfd, buf, MAXLINE, 0);

		char* filename = buf + 4; //skip over 'ret '
		int fd = open(filename, O_RDWR, 0);
		if (fd < 0) {
			printf("ERROR open");
			return;
		}

		int total_bytes_received = 0;
		while (total_bytes_received < rsize) {
			rc = recv(clientfd, buf, MAXLINE, 0);
			if (rc < 0) {
				close(fd);
				return;
			} else if(rc == 0) {
				close(fd);
				return;
			} else {
				total_bytes_received += rc;
				if(write(fd, buf, rc) < 0) {
					printf("%s", buf);
					exit(1);
				}
			}
		}
		close(fd);
		printf("File '%s' downloaded successfully\n", filename);
		return;
	}

	while (1) {
		rc = recv(clientfd, buf, MAXLINE, 0);
		if (rc > 0) {
			//decrypt(buf, buf);

			//write the received reply from the server
			if(write(1, buf, rc) < 0) {
				printf("%s", buf);
				exit(1);
			}
		} else if(rc == 0) {
			printf("end\n");
			return ;
		} else {
			printf("error in recv");
			exit(1);
		}
	}
}*/
