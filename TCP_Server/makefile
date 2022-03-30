CC = gcc
FLAGS = -Wall -g

all: tcpserver

tcpserver:
	$(CC) $(FLAGS) -o tcpserver tcpserver.c
	
.PHONY: clean all

clean: # Remove any file that might created.
	rm -f *.o *.a *.so *.gch shell
