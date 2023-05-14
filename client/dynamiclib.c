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
#include <errno.h>

#define MAXLINE 49152

//extern char* decrypt(char *encryption, char* original);
//extern char* encrypt (char* original, char* encryption);


//----------------------------------------FUNCTION DEFINITIONS--------------------------------------

int send_cmd(int fd, char *cmd) 
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

void receive_reply(int sockfd, char* buf)
{
	int rc;
	if (strstr(buf, "ret") != NULL) {
		char* filename = NULL;
		char* command = strdup(buf); //a copy of buf
		char* token = strtok(command, " ");
		
		if (strcmp(token, "ret") == 0) {
			filename = strtok(NULL, " ");
		}

		//receive the size of the file
		int file_size;
		int rsize = recv(sockfd, &file_size, MAXLINE, 0);
		if (rsize <= 0) {
			return;
		}
		rsize = file_size;
			
		int fd = open(filename, O_RDWR | O_CREAT, 0644);
		if (fd < 0) {
			printf("ERROR open");
			return;
		}

		int total_bytes_received = 0;
		while (total_bytes_received < rsize) {
			rc = recv(sockfd, buf, MAXLINE, 0);
			if (rc <= 0) {
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
		printf("File '%s' downloaded successfully\n", filename);
		free(command);
		close(fd);
		return;
	}

	while (1) {
		rc = recv(sockfd, buf, MAXLINE, 0);
		if (rc > 0) {
			//decrypt(buf, buf);

			//write the received reply from the server
			int written = 0;
			while (written < rc) {
				int n = write(1, buf + written, rc - written);
				if (n < 0) {
					printf("Error in write\n");
					exit(1);
				}
				written += n;
			}
			if (written == rc) {
				printf("Received and written %d bytes\n", written);
				break; // stop the loop if written bytes == received bytes
			}
		} else if(rc <= 0) {
			printf("errno = %d\n", errno);
			return ;
		} else {
			printf("error in recv");
			exit(1);
		}
	}
}
/*void receive_reply(int clientfd, char* buf)
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
