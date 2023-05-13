#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>

#define MAXLINE 4096

/*extern int send_cmd(int fd, char *cmd);
extern void receive_reply(int fd, char* buf);*/

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
void receive_reply(int fd, char* buf)
{
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

	char buf[MAXLINE];
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
