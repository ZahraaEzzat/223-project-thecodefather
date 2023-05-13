/*
 *
 * 'exit' implementation: when the function is called, it'll execute code cleanup and terminate the current state of the program gracefully. This code shoud statically linked with server.c (run "make")
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
extern char *dummy, *ext , *cmdn, *tmp;
//extern int clientfd;

void _exitt(int clientfd)
{
	free(dummy);
	free(cmdn);
	free(tmp);
	close(clientfd);
	exit(0);
}
