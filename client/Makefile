theCodeFather:
	gcc -c -fPIC dynamiclib.c -o dl.o
	ar rs lib.a \dl.o
	gcc -o c -g client.c lib.a
