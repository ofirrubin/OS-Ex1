#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <signal.h>

#include<unistd.h> 
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>

extern int errno;
#define MAX 64
#define PORT 10000

int client = -1;
int server = -1;

void handle(int sock)
{
	char buffer[MAX];
	while(1) {
		bzero(buffer, MAX);
		read(sock, buffer, sizeof(buffer));
		for(int i=0; i<MAX; i++)
			printf("%c", buffer[i]);
		bzero(buffer, MAX);
	}
}

void close_all(int code)
{
	if (client != -1)
		close(client);
	if (server != -1)
		close(server);
}

// Driver function
int main()
{
    	signal(SIGPIPE, SIG_IGN); // on linux to prevent crash on closing socket
	struct sockaddr_in servaddr, caddr;

	server = socket(AF_INET, SOCK_STREAM, 0);
	if (server == -1) {
		printf("Failed to create server socket\n");
		exit(0);
	}
	bzero(&servaddr, sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); // Broadcast listenning
	servaddr.sin_port = htons(PORT);

	if ((bind(server, (struct sockaddr*)&servaddr, sizeof(servaddr))) != 0) {
		printf("socket bind failed with error %d\n", errno);
		exit(0);
	}
	// Overide exit / pause to close connections
	signal(SIGINT, close_all);
	signal(SIGTSTP, close_all);
	if ((listen(server, 1)) != 0) { // Queue size of 1.
		printf("Listen failed...\n");
		exit(0);
	}
	
	socklen_t t = sizeof(caddr);
	client = accept(server, (struct sockaddr*)&caddr, &t);
	if (client < 0) {
		printf("Server failed to accept client...\n");
		exit(0);
	}
	
	handle(client);
	close_all(0);
}

