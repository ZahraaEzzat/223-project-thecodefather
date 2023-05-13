/*						SERVER
 *  
 *
 */



#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <netinet/tcp.h>
#include <pthread.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>

extern int receive_cmd (int fd, char *cmd);
extern void send_reply (int clientfd, char *cmd);


int main(int argc, char **argv)
{
	if (argc != 2) {
		printf("Usage: ./program <port>\n");
		exit(1);
	}

	struct sockaddr_in sockaddr, clientaddr;
	int sockfd, opt = 1;
	
	
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf ("Error creating socket: %s\n", strerror(errno));
		exit (1);
	}

	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		printf ("Error setting socket options: %s\n", strerror(errno));
		exit(1);
	} 

	memset (&sockaddr, 0, sizeof(sockaddr));
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(atoi(argv[1]));
	sockaddr.sin_addr.s_addr = INADDR_ANY;

	if (bind(sockfd, (struct sockaddr *) &sockaddr, sizeof(sockaddr)) < 0) {
		printf ("Error binding port & IP address: %s\n", strerror(errno));
		exit(1);
	}

	if (listen(sockfd , 5) == -1) {
		printf ("Error listening: %s\n", strerror(errno));
		exit(1);
	}

	printf ("Listening...\n");

/*	//initialize mutex
	if (pthread_mutex_init(&mutex, NULL) != 0) {
		printf("Error initializing mutex\n");
		exit(1);
	}*/


	while (1)
	{
		int clientfd;
		socklen_t cLen = sizeof(clientaddr);
		memset(&clientaddr, 0, sizeof(clientaddr));

		if ((clientfd = accept(sockfd , (struct sockaddr*) &clientaddr , &cLen)) < 0) {
			printf ("Error accepting request: %s\n", strerror(errno));
			exit(1);
		}
		puts("Client accepted");

		char buf[1024];
		int len;

		while (1) {
			memset(buf, 0, sizeof(buf));

			//receive the command
			len = receive_cmd(clientfd, buf);
			if (len < 0) {
				printf("Connection closed by client\n");
				break;
			} else if (len == 0) {
				continue;
			} else {
				buf[strcspn(buf, "\r\n")] = '\0';
				send_reply(clientfd, buf);
			}
		}
		close(clientfd);
	}
	close(sockfd);
	//pthread_mutex_destroy(&mutex);
	return 0;
}
