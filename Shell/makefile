CC = gcc
AR = ar
FLAGS = -Wall -g

all: main

# shellParsing header is included in libShell.
main: main.o libShell.a
	$(CC) $(FLAGS) -o main main.o libShell.a

libShell.a: libShell.o shellParsing.o
	$(AR) -rcs libShell.a libShell.o shellParsing.o

main.o: main.c libShell.h shellParsing.h
	$(CC) $(FLAGS) -c main.c

libShell.o: libShell.c libShell.h shellParsing.h
	$(CC) $(FLAGS) -c libShell.c

shellParsing.o: shellParsing.c shellParsing.h
	$(CC) $(FLAGS) -c shellParsing.c
	
.PHONY: clean all

clean: # Remove any file that might created.
	rm -f *.o *.a *.so *.gch main