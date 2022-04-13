#include <stdio.h> // Also required for networking
#include <stdlib.h> // Also required for networking

// Networking (for TCP localhost stdout)
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <sys/socket.h>
#include <signal.h> // So it won't crash

// All other shell requirements
#include <unistd.h>
#include "libShell.h" 
#include "shellParsing.h"
#include <errno.h>

extern int errno;

#define IP "127.0.0.1" // localhost aka loopback
#define DEFAULT_PORT 10000 // Didn't understood whether the syntax is "tcp port" or "tcp <port-number>" thus accepting both.

void local(int default_stdout, int socket_fd) // Close the socket if open and change stdout to defaulted one.
{
	if (socket_fd != -1)
		close(socket_fd);
	dup2(default_stdout, 1);
}

int remote(int port) // Returns socket fd if connected
{
	int sockfd;
	struct sockaddr_in connAddr;
   
	sockfd = socket(AF_INET, SOCK_STREAM, 0); // create socket (TCP, IPv4)
	if (sockfd == -1) {
		return -1;
	}
	connAddr.sin_family = AF_INET; // Change sockets settings, IPv4
	connAddr.sin_addr.s_addr = inet_addr(IP); // Set IP (convert string to the struct)
	connAddr.sin_port = htons(port); // Convert port from host byte-order to network byte-order.
	if (connect(sockfd, (struct sockaddr *)&connAddr, sizeof(connAddr)) != 0) { // Connect to server
		printf("Connection failed with error number: %d\n", errno);
		return -1;
	}
	else
		return sockfd;
}

int socket_is_alive(int socket_fd) // Check whether a socket is connected https://stackoverflow.com/a/4142038
{
	int error = 0;
	socklen_t len = sizeof (error);
	int retval = getsockopt (socket_fd, SOL_SOCKET, SO_ERROR, &error, &len);
	if (retval != 0 || error != 0) {
		return 0;
	}
	return 1;
}

int shell_loop(int system_by_few)
{

    	signal(SIGPIPE, SIG_IGN); // on linux to prevent crash on closing socket
	int console = dup(1); // Save default stdout: https://stackoverflow.com/questions/11042218/c-restore-stdout-to-terminal
	int stdout_= console;
	int socket_fd = -1;
	// Changing the stdout by:
	dup2(stdout_, 1); // 1 is stdout
	char *cmd = NULL;
	int cmd_len = 0;
	while (cmd_len == 0)
	{	
		// We want to make sure that if we are connected to a socket it's still alive:
		if (socket_fd != -1 && !socket_is_alive(socket_fd))
		{
			println("Seems like the socket is disconnected.. changing back to local");
			local(console, socket_fd); // try to shutdown socket, change back to socket.
			socket_fd = -1;
		}
		if (stdout_ == console) // Output prompt only if in console mode
			print(""); // Prints empty shell
		get_command(&cmd, &cmd_len);
		
		if (cmd_eq(cmd, "exit") || cmd_eq(cmd, "bye")){ // Check for program exit
			free(cmd);
			printf("\nshell: Good bye!\n");
			return 0;
		}
		
		if (cmd_sw(cmd, "echo ")){ // Print everything after the echo
			println_from(cmd, sizeof("echo"));
		}
		else if (cmd_eq(cmd, "cwd")) // Print cwd
			print_cwd();
		else if (cmd_eq(cmd, "dir")) // List files in the directory
			dir();
			
		else if (cmd_sw(cmd, "cd ")) // Change current working directory
			cd(cmd + sizeof("cd"));

		else if (cmd_sw(cmd, "delete ")) // Delete a file
			delete_file(cmd + sizeof("delete"));
			
		else if (cmd_sw(cmd, "copy ")) // Copy a file
			copy(cmd + sizeof("copy"));
		
		else if (cmd_eq(cmd, "clear") || cmd_eq(cmd, "cls")) // Clear screen
			clear();
		else if (cmd_eq(cmd, "local"))
		{
			stdout_ = console;
			local(stdout_, socket_fd);
			socket_fd = -1;
		}	
		else if (cmd_sw(cmd, "tcp "))
		{
			int port = DEFAULT_PORT;

			if (!cmd_eq(cmd + sizeof("tcp"), "port")) // Check if default port or user input
				port  = atoi(cmd + sizeof("tcp"));

			printf("shell: Connecting to localhost:%d\n", port);
			socket_fd = remote(port); // Connect to server
			if (socket_fd != -1) // Check connection
			{
				println("Connected to TCP Server, redirecting output..."); // Connected, change
				dup2(socket_fd, 1);
				stdout_ = socket_fd;
			}
			else
				println("Error, check server status and try again."); // Failed
			
		}	
		else
		{
			if (system_by_few) // system by few (fork, exec, wait)
				systm(cmd);
			else
				system(cmd);
		}
		// Get ready for next command:
		free(cmd);
		cmd = NULL;
		cmd_len = 0;
	}
	printf("%d\n", cmd_len);
	return 0;
}

int main()
{
	// Answers:
	// chdir() is system function as is listed in system calls at: [refer as system calls manual] https://man7.org/linux/man-pages/man2/syscalls.2.html
	
	// at copy function, we are using fopen() fwrite() / fread() and fclose() which are library functions that may use system functions at the backend. Reasons:
	//1. The functions are not listed at:  system calls manual (listed above) 
	//2. As described here: https://stackoverflow.com/questions/2668747/system-call-vs-function-call 
	//3. By using strace you can see that the functions themself are not listed but rather their backend (read, fstat, openat etc.)

	// system() function is system call wrapper in c, it's not described at the system calls manual but it uses fork&exec&wait which are system calls - unlike fwrite() that may not require system calls at background, this will always use system calls at the backend.
	// In addition, by looking at strace you can see the backend functions and not the function it's self. [make sure to run the part with the actual system() call and not by fork to see your by yourself.
	
	// unlink() is a system function as it's described at the system calls manual, In addition by looking at the manual we can see that unlink is "symbolink" for using unlinkat in a certain way - We can see that unlinkat is a system call by also looking at strace.
	
	// Comment Everything except what you want to run, I didn't make it in makefile because you requested in comments.
	
	//System by fork/exec/wait:
	return shell_loop(1);
	
	//System by system() call
	//return shell_loop(0);
	// Print CWD:
	//print_cwd();
	
}
