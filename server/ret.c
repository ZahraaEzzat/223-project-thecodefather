/*
 *
 retrieve
 *
 */
#include <stdio.h>
#include <dirent.h>
#include <fnmatch.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>

void ret(int clientfd, char* filename) {
puts("I AM RETRIEVE");
	DIR *dir;
	struct dirent *ent;
	struct stat file_stat;

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
	send(clientfd, "0", sizeof("0"), 0);
	send(clientfd, "No file to retrieve", sizeof("No file to retrieve"), 0);

	closedir(dir);
}
