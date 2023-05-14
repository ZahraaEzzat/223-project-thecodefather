#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>


//extern int send_cmd(int fd, char *cmd);
//extern void receive_reply(int fd, char* buf);
#define MAXLINE 12288
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
		} else if(rc == 0) {
			printf("end\n");
			return ;
		} else {
			printf("error in recv");
			exit(1);
		}
	}
}

int main(int argc, char **argv)
{
	if (argc != 3) {
		printf("Usage: ./client <ip> <port>\n");
		exit(1);
	}

	int sockfd;
	struct sockaddr_in server_addr;

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf ("Error creating socket: %s\n", strerror(errno));
		exit (1);
	}

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[2]));

	if (inet_pton(AF_INET, argv[1], &server_addr.sin_addr) <= 0) {
		printf("Invalid IP address: %s\n", argv[1]);
		exit(1);
	}

	if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		printf("Connection failed: %s\n", strerror(errno));
		exit(1);
	}

	printf("Connected to server\n");

	char buf[4096];
	int len;
	while (1) {
		memset(buf,0,sizeof(buf));

		write(2, "TheCodeFather> ", sizeof("TheCodeFather> "));

		len = read(1, buf, sizeof(buf));
		if (len < 0) {
			printf("Error reading input\n");
			exit(1);
		} else {
			buf[strcspn(buf, "\r\n")] = '\0';

			//send the command to the server
			int sc = send_cmd(sockfd, buf);
			if (sc < 0)
				continue;

			//receive reply from the server
			receive_reply(sockfd, buf);
			memset(buf, 0, sizeof(buf));
		}
	}
	close(sockfd);
	return 0;
}
