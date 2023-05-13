#include <stdio.h>
#include <sys/socket.h>
#include <string.h>


void help(int clientfd)
{
	char buffer[] = "\n\n\t\t\t\t\t\t\t\t\tHelp Manual\n\t\t\t\t\t\t   You may invoke 'help' with the following commands:\n\n\t\t\t\t\t\t\t\t\t1. help\n\t\t\t\t\t\t\t\t\t2. cat\n\t\t\t\t\t\t\t\t\t3. ls\n\t\t\t\t\t\t\t\t\t4. exit\n\n\n\n> help\n\n\tSynopsis: help [COMMAND]\n\n\tDescription: Display a short description on built-in commands\n\n\n\n\n> cat\n\n\tSynopsis: cat [filename(s)]\n\n\tDescription: concatenate files to standard output\n\n\tIf no file is given, it infinitely reads user input and displays it\n\ton standard output (Use CTRL + C to terminate)\n\n\tUse '*' wildcard to print the content of files that share the same sequence of characters\n\n\n\tExamples: \n\n\t1- Files with the same extension: cat *.c\n\t2- Pattern matching: cat *tmp*  => contents of files that have the name 'tmp' will be displayed\n\t3- cat ../*.c => concatenates all C files from the parent directory to standard output\n\t4- cat filename.pl => displays content of source file\n\n\n\n\n> ls\n\n\tSynopsis: ls [filename(s)]\n\n\tDescription: display file information within working directory [permissions/# of links/ownership/file size/last edited date/file name]\n\n\tIf command is run as such: ls OR ls *, the info of every file within the working directory will be displayed alphabetically\n\n\n\tExamples: \n\n\t1- ls tmp.c => shell displays the name of the file\n\t2- ls .. => names of files in parent directory are displayed\n\n\n\n\n> exit\n\n\tNothing interesting about it to be honest\n\n\tSynopsis: exit\n\n\tDescription: clean up and gracefully terminate current state of program\n\n\n\n\n";

	send(clientfd, buffer, strlen(buffer), 0);
}
