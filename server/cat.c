/*
 * Implementation of 'cat' shell command: 
 *
 * 'cat' concatenates files into standard output. The mechanism is affected by specifying patterns the files share.
 * Running 'cat *.c' will output the contents of all C files (since they all end with '.c').
 * There are two general cases in which cat is invoked: running the command in the current working directory and running the command through parent directories. Both will be implemented using string functions (e.g., strtok + strchr) and the 'dirent.h' header file. 
 *
 * This code will be statically linked with server.c (run 'make')
 */

#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/socket.h>

char *dummy;
//extern int clientfd;
//extern void send_reply (int fd, char *cmd);

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
		send(clientfd, "No file name was passed\n", strlen("No file name was passed\n"), 0);
		return;
	}

	// subcase 2: cat *
	if (strcmp(filename, "*") == 0)	{
		dp = opendir(".");
		if (!dp) {
			perror ("Error opening directory\n");
			return;
		}

		// read files in directory and put them in buffer
		size_t buf_size = 1024; // initial buffer size
		buf = malloc(buf_size);
		if (buf == NULL) {
			perror("Error allocating buffer\n");
			closedir(dp);
			return;
		}
		ssize_t nread;

		while ((fentry = readdir(dp)) != NULL) {
			if (strcmp(fentry->d_name, ".") == 0 || strcmp(fentry->d_name, "..") == 0) {
				continue; // skip current and parent directories
			}
			fd = open(fentry->d_name, O_RDONLY);
			if (fd < 0) {
				perror("Error opening file\n");
				continue;
			}
			if (fstat(fd, &filestat) < 0) {
				printf("Error fstat\n");
				closedir(dp);
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
						return;
					}
				}
			}
			if (nread < 0) {
				perror("Error reading file\n");
				closedir(dp);
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


		/*	if ((buf = mmap (NULL, filestat.st_size, PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED) {
				printf("Error mapping\n");
				return;
			}

			buffer = realloc(output_buffer, output_buffer_size + filestat.st_size);
			if (buffer == NULL) {
				perror("Error allocating output buffer\n");
				return;
			}

			memcpy(buffer + output_buffer_size, buffer, filestat.st_size);
			output_buffer_size += filestat.st_size;
			if (munmap(buf, filestat.st_size) < 0) {
				printf("Error munmap\n");
				return;
			}
			close(fd);
		}
		closedir(dp);

		send(clientfd, output_buffer, output_buffer_size, MSG_WAITALL);
		free(buffer);
	}
	if (strstr(filename, "/") == NULL) {
		dp = opendir(".");
		if (!dp) {
			perror("Error opening directory\n");
			exit(-1);
		}*/


	// subcase 3: if wildcard character is used
	dp = opendir(".");
	if (dp == NULL) {
		perror("Error opening directory\n");
		return;
	}
	if (strstr(filename, "*") != NULL) {
		char *tok = (char *) malloc(sizeof(char)*(strlen(filename)+1));
		snprintf(tok, strlen(filename)+1, "%s", filename);
		dummy = tok;
		tok = strtok(tok, "*");
		while (tok != NULL) {
			while ((fentry = readdir(dp)) != NULL) {
				if(strstr(fentry->d_name, tok) != NULL) {
					fd = open(fentry->d_name, O_RDONLY); 
					if (fd < 0) {
						perror("Error opening file\n");
						free(tok);
						closedir(dp);
						return;
					}
       	                		if (fstat(fd, &filestat) < 0) {
                       	        		printf("Error fstat\n");
                       	        		free(tok);
                       	        		close(fd);
                       	        		closedir(dp);
                               			return;
					}
			                char *file_buffer = mmap(NULL, filestat.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
					if (file_buffer == MAP_FAILED) {
						printf("Error mapping\n");
						close(fd);
                       	        		closedir(dp);
                                		return;
                        		}
                        		buf = realloc(buf, output_buffer_size + filestat.st_size);
					if (buf == NULL) {
						perror("Error allocating memory\n");
						close(fd);
                       	        		closedir(dp);
						return;
					}
					memcpy(buf + output_buffer_size, file_buffer, filestat.st_size);
					output_buffer_size += filestat.st_size;
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
			tok = strtok (NULL, "*");
		}
		free(tok);
		free(dummy);
	}
	closedir(dp);
	
	if (flag) {
		//send the buffer to the client
		if (send(clientfd, buf, output_buffer_size, 0) < 0) {
			perror("Error sending data\n");
			return;
		}
	}
	free(buf);
	return;

	// subase 4: cat filename(s)
	dp = opendir(".");
	if (dp == NULL) {
		perror("Error opening directory\n");
		return;
	}
	char *buffer;
	while ((fentry = readdir(dp)) != NULL) {
		if (strcmp(fentry->d_name, filename) == 0) {
			flag = 1;
			fd = open(fentry->d_name, O_RDONLY);
			if (fd < 0) {
				perror("Error opening file\n");
				return;
			}
			if (fstat(fd, &filestat) < 0) {
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
		return;
	}
	send(clientfd, buffer, filestat.st_size, MSG_WAITALL);
	free(buffer);
	closedir(dp);
	return;
	
	/*else {
		while ((fentry = readdir(dp)) != NULL) {
                       	if (strcmp(fentry->d_name, filename) == 0)
                               	flag = 1;
               	}
		rewinddir(dp);
               	if (flag == 0) {
			char file[50];
                       	sprintf(file,"cat: '%s': no such file or directory\n", filename);
                       	send(clientfd,file, strlen(file), MSG_WAITALL);
			return;
               	}
		while ((fentry = readdir(dp)) != NULL) {
			if (strcmp(fentry->d_name, filename) == 0) {
				fd = open(fentry->d_name, O_RDONLY);
				if (fd < 0) {
					perror("Error opening file\n");
					exit(-1);
				}
                       		if (fstat (fd, &filestat) < 0) {
                               		printf("Error fstat\n");
                               		exit(1);
                       		}
                       		if ((buffer = mmap (NULL, filestat.st_size, PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED) {
                               		printf("Error mapping\n");
                               		exit(1);
                       		}
                       		send(clientfd, buffer, filestat.st_size, MSG_WAITALL);
                       		if (munmap(buffer, filestat.st_size) < 0) {
                               		printf ("Error munmap\n");
                               		exit (1);
                       		}				
				close(fd);
			}
		}
	}
	closedir(dp);*/
}
	
