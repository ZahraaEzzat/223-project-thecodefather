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



//extern int receive_cmd (int fd, char *cmd);
//extern void send_reply (int clientfd, char *cmd);

int receive_cmd (int clientfd, char *cmd)
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

char *dummy;
void cat(int clientfd, char *filename)
{
	struct dirent *fentry;
	struct stat filestat;
	DIR *dp;
	char *buf = NULL;
	int flag = 0, fd, output_buffer_size = 0;

	// Case 1: current directory
	// subcase 1: cat NULL (no file name passed)
	if(filename == NULL) {
		send(clientfd, "No file name was passed\n ", strlen("No file name was passed\n"), 0);
		return;
	}

	// subcase 2: cat * (view the contents of all files in the current directory)
	size_t buf_size = 4086; // initial buffer size
	if (strcmp(filename, "*") == 0)	{
		dp = opendir(".");
		if (!dp) {
			perror ("Error opening directory\n");
			return;
		}

		// read files in directory and put them in buffer
		buf = malloc(buf_size);
		if (buf == NULL) {
			perror("Error allocating buffer\n");
			closedir(dp);
			return;
		}
		ssize_t nread;
		struct stat filestat;

		while ((fentry = readdir(dp)) != NULL) {
			if (strcmp(fentry->d_name, ".") == 0 || strcmp(fentry->d_name, "..") == 0) {
				continue; // skip current and parent directories
			}
			char filepath[1024];
			snprintf(filepath, 1024, "%s/%s", ".", fentry->d_name);
			if (!S_ISREG(filestat.st_mode)) {
				//skipping directories
				continue;
			}
			fd = open(fentry->d_name, O_RDONLY);
			if (fd < 0) {
				continue;
			}
			if (fstat(fd, &filestat) < 0) {
				close(fd);
				continue;
			}	

			while ((nread = read(fd, buf + output_buffer_size, buf_size - output_buffer_size)) > 0) {
				output_buffer_size += nread;
				if (output_buffer_size >= buf_size) {
					buf_size *= 2; //doubl buffer size
					buf = realloc(buf, buf_size);
					if (buf == NULL) {
						perror("Error reallocating buffer\n");
						closedir(dp);
						close(fd);
						return;
					}
				}
			}
			if (nread < 0) {
				printf("Error reading file\n");
				close(fd);
				continue;
			}
			close(fd);
		}
		closedir(dp);

		// send buffer to client
		if (send(clientfd, buf, output_buffer_size, 0) < 0) {
			perror("Error sending data to client\n");
			free(buf);
			return;
		}
		free(buf);
		return;
	}


	// subcase 3: cat file.* (a wildcard character is used)
	//if the filename argument contains a wildcard character
	if (strstr(filename, "*") != NULL) {
		dp = opendir(".");
		if (dp == NULL) {
			perror("Error opening directory\n");
			return;
		}

		char *tok = (char *) malloc(sizeof(char)*(strlen(filename)+1));
		//copy filename to tok
		snprintf(tok, strlen(filename)+1, "%s", filename);
		dummy = tok;
		tok = strtok(tok, "*");
		while (tok != NULL) {
			while ((fentry = readdir(dp)) != NULL) { //iterate over each file in .
				if(strstr(fentry->d_name, tok) != NULL) { //if filename contains the token
					char *dot = strrchr(fentry->d_name, '.'); //get the extension
					if(strcmp(dot, ".o") != 0) {
						fd = open(fentry->d_name, O_RDONLY); //open it
						if (fd < 0) {
							perror("Error opening file\n");
							free(tok);
							closedir(dp);
							return;
						}
       	                			if (fstat(fd, &filestat) < 0) { //get stats abt the file
                       	        			printf("Error fstat\n");
                       	        			free(tok);
                       	        			close(fd);
                       	        			closedir(dp);
                               				return;
						}

						//map the file into memory
			                	char *file_buffer = mmap(NULL, filestat.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
						if (file_buffer == MAP_FAILED) {
							printf("Error mapping\n");
							close(fd);
                       	        			closedir(dp);
                                			return;
                        			}

						buf_size *= 2; //doubl buffer size
	                        		buf = realloc(buf, output_buffer_size + filestat.st_size);
						if (buf == NULL) {
							perror("Error allocating memory\n");
							close(fd);
	                       	        		closedir(dp);
							return;
						}

						//copy the file content to the output buffer
						memcpy(buf + output_buffer_size, file_buffer, filestat.st_size);
						output_buffer_size += filestat.st_size;

						//unmap the file from memory
						if (munmap(file_buffer, filestat.st_size) < 0) {
							printf("Error munmap\n");
							close(fd);
	                       	        		closedir(dp);
							return;
						}
	                			close(fd);
	                			flag = 1;
					}
				}
			}
			tok = strtok (NULL, "*");
		}
		free(tok);
		free(dummy);
		closedir(dp);
	}
	
	if(flag) {
		//send the buffer to the client
		if (send(clientfd, buf, output_buffer_size, 0) < 0) {
			perror("Error sending data\n");
			return;
		}
		free(buf);
		return;
	}


	// subase 4: cat filename(s)
	dp = opendir(".");
	if (dp == NULL) {
		perror("Error opening directory\n");
		return;
	}
	char *buffer;
	while ((fentry = readdir(dp)) != NULL) { //iterate over files
		if (strcmp(fentry->d_name, filename) == 0) {
			flag = 1;
			fd = open(fentry->d_name, O_RDONLY); //open the file
			if (fd < 0) {
				perror("Error opening file\n");
				return;
			}
			if (fstat(fd, &filestat) < 0) { //get file's stats
				printf("Error fstat\n");
				return;
			}
			if ((buffer = (char*) malloc(filestat.st_size)) == NULL) {
				printf("Error allocating memory\n");
				exit(1);
			}
			if (read(fd, buffer, filestat.st_size) < 0) {
				printf("Error reading file\n");
				exit(1);
			}
			close(fd);
			break;
		}
	}
	if (flag == 0) {
		char file[50];
		sprintf(file, "cat: '%s': no such file or directory\n", filename);
		send(clientfd, file, strlen(file), MSG_WAITALL);
		closedir(dp);
		return;
	}
	send(clientfd, buffer, filestat.st_size, MSG_WAITALL);
	free(buffer);
	closedir(dp);
	return;
}
	







char *cmdn;

void parse_cmd(int clientfd, char *cmd)
{
	printf("Parse: %s\n", cmd);
	cmd[strlen(cmd)]='\0';
	char *arg = strtok(cmd, " ");
	
	cmdn = (char *)malloc(sizeof(char)*100);
        strcpy(cmdn, arg);

	if ((strcmp(cmdn, "help")) == 0)
		/*help(clientfd)*/;

	else if (strcmp(cmdn, "exit") == 0)
		/*_exitt(clientfd)*/;

	/*else if ((strcmp(cmdn , "cat") == 0) || (strcmp(arg , "ls")) == 0 || (strcmp(arg , "ret")) == 0) 
	{*/
		arg = strtok(NULL, " ");

		if ((strcmp(cmdn, "cat") == 0) && arg == NULL)
			cat(clientfd, NULL);

	/*	else if ((strcmp(cmdn, "ls") == 0) && arg == NULL)
			ls_cmd(clientfd, NULL);

		*/else if (strcmp(cmdn, "cat") == 0)
		{
			while (arg != NULL)
			{
				cat(clientfd, arg);
				arg = strtok(NULL, " ");
			}
		}

		/*else if (strcmp(cmdn, "ls") == 0)
			ls_cmd(clientfd, arg);

		else if (strcmp(cmdn, "ret") == 0)
			//retrieve(arg)
			;
	}*/

	else
	{ 
		send(clientfd,"Not valid\n", strlen("Not valid\n"), 0);
		
	}
}

void send_reply(int clientfd, char *cmd)
{
	//encrypt(cmd,cmd);
	parse_cmd(clientfd, cmd);
}

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
