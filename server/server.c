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
#include <fcntl.h>
#include <string.h>
#include <fnmatch.h>
#include <sys/mman.h>

/*extern int receive_cmd (int fd, char *cmd);
extern void send_reply (int clientfd, char *cmd);*/
void help(int clientfd)
{
	char buffer[] = "\n\n\t\t\t\t\t\t\t\t\tHelp Manual\n\t\t\t\t\t\t   You may invoke 'help' with the following commands:\n\n\t\t\t\t\t\t\t\t\t1. help\n\t\t\t\t\t\t\t\t\t2. cat\n\t\t\t\t\t\t\t\t\t3. ls\n\t\t\t\t\t\t\t\t\t4. exit\n\n\n\n> help\n\n\tSynopsis: help [COMMAND]\n\n\tDescription: Display a short description on built-in commands\n\n\n\n\n> cat\n\n\tSynopsis: cat [filename(s)]\n\n\tDescription: concatenate files to standard output\n\n\tIf no file is given, it infinitely reads user input and displays it\n\ton standard output (Use CTRL + C to terminate)\n\n\tUse '*' wildcard to print the content of files that share the same sequence of characters\n\n\n\tExamples: \n\n\t1- Files with the same extension: cat *.c\n\t2- Pattern matching: cat *tmp*  => contents of files that have the name 'tmp' will be displayed\n\t3- cat ../*.c => concatenates all C files from the parent directory to standard output\n\t4- cat filename.pl => displays content of source file\n\n\n\n\n> ls\n\n\tSynopsis: ls [filename(s)]\n\n\tDescription: display file information within working directory [permissions/# of links/ownership/file size/last edited date/file name]\n\n\tIf command is run as such: ls OR ls *, the info of every file within the working directory will be displayed alphabetically\n\n\n\tExamples: \n\n\t1- ls tmp.c => shell displays the name of the file\n\t2- ls .. => names of files in parent directory are displayed\n\n\n\n\n> exit\n\n\tNothing interesting about it to be honest\n\n\tSynopsis: exit\n\n\tDescription: clean up and gracefully terminate current state of program\n\n\n\n\n";

	send(clientfd, buffer, strlen(buffer), 0);
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
int myfnmatch(char *str, char *pattern);
//extern int sockfd, clientfd;
extern void send_reply (int fd, char *cmd);
char *tmp;
void ls_cmd(int clientfd, char *arg)
{
	DIR *dir;
	struct dirent *dent;
	struct stat status;
	char *dirname;
	char *pattern;
	char path[1024];
	char buffer[256];

	memset(buffer, 0, sizeof(buffer));

	//check if arg is NULL or " "
	if (arg == NULL || arg == " ")
		arg = ".";

	//check if arg is a file (ls filename)
	if (stat(arg, &status) == 0 && S_ISREG(status.st_mode)) {
		strcat(arg, "\n");
		send(clientfd, arg, sizeof(arg), 0);
		return;
	}

	//check if arg is a directory
	if (stat(arg, &status) == 0 && S_ISDIR(status.st_mode)) {
		if ((dir = opendir(arg))) {
			while ((dent = readdir(dir)) != NULL) {
				if (strcmp(dent->d_name, ".") == 0 || strcmp(dent->d_name, "..") == 0)
					continue;
				strcat(buffer, dent->d_name);
				strcat(buffer, "\n");
			}
			closedir(dir);
			send(clientfd, buffer, strlen(buffer), 0);
			return;
		} else {
			sprintf(buffer, "Error opening directory: %s\n", strerror(errno));
			send(clientfd, buffer, strlen(buffer), 0);
			return;
		}
	} else {
		dirname = arg;
	}

	//split arg into dirname and pattern
	//if arg == /home/dir1/dir2/  dirname == /home/dir1/dir2 pattern == dir2
	//if arg == file.txt  dirname == .  pattern == file.txt
	tmp = malloc(strlen(arg) + 2);

	strcpy(tmp, arg);
	dirname = strtok(tmp, "/");
	pattern = strtok(NULL, "/");
	
	if (pattern == NULL) {
		pattern = dirname;
		dirname = ".";
	} else if (dirname[0] == '*') {
		//pattern is in the current directory
		dirname = ".";
		pattern = arg;
	} else {
		//pattern contains directory components
		int len = strlen(arg) - strlen(pattern) - 1;

		dirname[len] = '\0';
		pattern++;
	}

	//open directory and iterate over files
	if ((dir = opendir(dirname))) {
		while ((dent = readdir(dir)) != NULL) {
			if (strcmp(dent->d_name, ".") == 0 || strcmp(dent->d_name, "..") == 0)
				continue;

			// check if file name matches pattern
			if (myfnmatch(dent->d_name, pattern)) {
				sprintf(path, "%s/%s", dirname, dent->d_name);
				if (stat(path, &status) == -1) {
					sprintf(buffer, "Error: %s\n", strerror(errno));
					send(clientfd, buffer, strlen(buffer), 0);
					return;
				}
				// print file name with appropriate formatting
				if (S_ISDIR(status.st_mode)) {
					strcat(buffer, dent->d_name);
					strcat(buffer, "\n");
					//printf("%s/\n", dent->d_name);
				} else {
					strcat(buffer, dent->d_name);
					strcat(buffer, "\n");
					//printf("%s\n", dent->d_name);
				}
			}
		}
		send(clientfd, buffer, strlen(buffer), 0);
		closedir(dir);
	} 
	
	else {
		sprintf(buffer, "Error opening directory: %s\n", strerror(errno));
		send(clientfd, buffer, strlen(buffer), 0);
		return;
	}
}
void ret(int clientfd, char* filename) {
puts("I AM RETRIEVE");
	DIR *dir;
	struct dirent *ent;
	struct stat file_stat;

	if (strcmp(filename, "*") == 0) { // retrieve all files
		dir = opendir(".");
		if (dir == NULL) {
			printf("Failed to open directory\n");
			return;
		}

		// loop through all files in directory
		while ((ent = readdir(dir)) != NULL) {
			if (ent->d_type == DT_REG) { // only regular files
				// get file stats
				if (stat(ent->d_name, &file_stat) == -1) {
					printf("Failed to get file stats\n");
					return;
				}

				// send file size to client
				if (send(clientfd, &file_stat.st_size, sizeof(file_stat.st_size), 0) == -1) {
					printf("Failed to send file size\n");
					return;
				}

				// send file content to client
				int file_fd = open(ent->d_name, O_RDONLY);
				char buffer[1024];
				ssize_t read_size;
				while ((read_size = read(file_fd, buffer, sizeof(buffer))) > 0) {
					if (send(clientfd, buffer, read_size, 0) == -1) {
						printf("Failed to send file content\n");
						return;
					}
				}
				close(file_fd);
			}
		}
		closedir(dir);
		return;
	} else { // retrieve a single file
		dir = opendir(".");
		if (dir == NULL) {
			printf("Failed to open directory\n");
			return;
		}

		//loop through all files in directory
		while ((ent = readdir(dir)) != NULL) {
			if (fnmatch(filename, ent->d_name, FNM_NOESCAPE) == 0) {
				//get file stats
				if (stat(ent->d_name, &file_stat) == -1) {
					printf("Failed to get file stats\n");
					return;
				}

				//send file size to client
				if (send(clientfd, &file_stat.st_size, sizeof(file_stat.st_size), 0) == -1) {
					printf("Failed to send file size\n");
					return;
				}
				//send file content to client
				int file_fd = open(ent->d_name, O_RDONLY);
				char buffer[1024];
				ssize_t read_size;
				while ((read_size = read(file_fd, buffer, sizeof(buffer))) > 0) {
					if (send(clientfd, buffer, read_size, 0) == -1) {
						printf("Failed to send file content\n");
						return;
					}
				}
				close(file_fd);
			}
		}
		return;
	}
	send(clientfd, "No file to retrieve", sizeof("No file to retrieve"), 0);
	send(clientfd, "No file to retrieve", sizeof("No file to retrieve"), 0);
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
		help(clientfd);

	else if (strcmp(cmdn, "exit") == 0)
		/*_exitt(clientfd)*/;

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
