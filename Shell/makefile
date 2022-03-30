CC = gcc
AR = ar
FLAGS = -Wall -g

all: shell

# shellParsing header is included in libShell.
shell: shell.o libShell.a
	$(CC) $(FLAGS) -o shell shell.o libShell.a

libShell.a: libShell.o shellParsing.o
	$(AR) -rcs libShell.a libShell.o shellParsing.o

shell.o: shell.c libShell.h shellParsing.h
	$(CC) $(FLAGS) -c shell.c

libShell.o: libShell.c libShell.h shellParsing.h
	$(CC) $(FLAGS) -c libShell.c

shellParsing.o: shellParsing.c shellParsing.h
	$(CC) $(FLAGS) -c shellParsing.c
	
.PHONY: clean all

clean: # Remove any file that might created.
	rm -f *.o *.a *.so *.gch shell
