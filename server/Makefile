theCodeFather:
	gcc -g -c -fPIC cat.c -o cat.o
	gcc -g -c -fPIC _exitt.c -o _exit.o
	gcc -g -c -fPIC help.c -o help.o
	gcc -g -c -fPIC ls.c -o ls.o
	gcc -g -c -fPIC myfnmatch.c -o myfnmatch.o
	gcc -g -c -fPIC parse_cmd.c -o parse.o
	gcc -g -c -fPIC dynamiclib.c -o dl.o
	ar rs lib.a \cat.o _exit.o help.o ls.o myfnmatch.o parse.o dl.o
	gcc -o server -g server.c lib.a
#	gcc -shared -o server cat.o _exit.o help.o ls.o myfncmatch.o parse.o -L. -ldl
#	gcc -o server server.c -L. -ldl
