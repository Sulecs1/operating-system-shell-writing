all:
	chmod a+x main.c
	gcc -o Shell main.c
clean:
	rm *o Shell
