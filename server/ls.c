/* 'ls' implementation:
 *
 * The ls command displays file information within the working directory (owner, permissions, date of modification, # of links, file name, etc...).
 *
 *
 * The implementation can be invoked as such:  ls *.ext (wildcard character), ls filename.ext, ls, and so on. The function uses the 'dirent.h' header file to parse the directory.
 *
 *
 * This code will be statically linked with server.c (run "make")
 *
 */


#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fnmatch.h>
#include <sys/socket.h>
#include <errno.h>

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
