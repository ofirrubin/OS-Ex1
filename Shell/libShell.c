#include "shellParsing.h"
#include "libShell.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

extern int errno; //https://www.tutorialspoint.com/cprogramming/c_error_handling.htm

#include <dirent.h>
#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE
#endif
#include <fcntl.h>
#include <sys/stat.h>
#undef _POSIX_SOURCE

void dir(void){
	// Get current working dir
	char cwd[256];
	if (getcwd(cwd, sizeof(cwd)) == NULL)
		println("Error getting directory info (cwd)");
	else
	{
		// Based on: https://www.ibm.com/docs/en/i/7.4?topic=ssw_ibm_i_74/apis/readdir.htm
		
		DIR *dir;
		struct dirent *entry;

		if ((dir = opendir(cwd)) == NULL)
			println("Error getting directory info (opendir)");
		else {
			println("---------------------------");
			while ((entry = readdir(dir)) != NULL)
				println(entry->d_name);
			closedir(dir);
			println("---------------------------");
		}
	}
	
}

void cd(char *path){ // Change current working directory Also allows cd .. etc.
	// Based on: https://www.ibm.com/docs/en/zos/2.3.0?topic=functions-chdir-change-working-directory#rtchd
	if (chdir(path) != 0)
		println("Error; No such file or directory");
}

void clear(void){ // Clear screen
	system("clear");
}

void delete_file(char *fname) // Remove file or a directory
{
	if (unlink(fname) != 0)
		printf("shell: Erorr deleting file with error %d\n", errno);
}


void print_cwd(){
	// Based on: https://www.ibm.com/docs/en/zos/2.3.0?topic=functions-getcwd-get-path-name-working-directory
	char cwd[256];
	if (getcwd(cwd, sizeof(cwd)) == NULL)
		println("Error getting current working directory");
	else
		println(cwd);
}

void copy(char *command){ // Copy files
	char *src = NULL;
	char *dst = NULL;
	double_quote(command, &src, &dst, 1); // 1= 'exception' over 2 arguments
	if (!src || !dst) // printed error already
		return;
	FILE *s = fopen(src, "rb");
	if (s == NULL)
	{
		println("Please make sure the source file exists");
		return;
	}
	// Check if destination exists already: https://stackoverflow.com/a/230068
	if (access( dst, F_OK ) == 0 )
	{
		println("Destination already exists. Do you want to overwrite? (Y / any other to cancel):");
		char c = getchar();
		if (c == 'Y' || c == 'y')
		{
		// Delete the file
			delete_file(dst);
		}
		else
		{
			println("Aborting.");
			fclose(s);
			return;
		}
	}
	println(dst);
	FILE *d = fopen(dst, "wb+"); // Create & Write bytes
	if (d == NULL)
	{
		print("Error openning/creating destination file");
		printf(" (%s) error: %d\n", dst, errno);
		fclose(s);
		return;
	}
	char *buffer[256];
	int r_size =0; // Read size
	while ((r_size = fread(&buffer, sizeof(char), sizeof(buffer), s)) > 0){
		if (fwrite(buffer, sizeof(char), r_size, d) != r_size)
		{
			println("Failed writing to the file");
			fclose(s);
			fflush(d);
			fclose(d);
			return;
		}
	}
	fclose(s);
	fflush(d);
	fclose(d);
}

void systm(char *cmd){ // Based on: https://www.ibm.com/docs/en/zos/2.3.0?topic=functions-waitpid-wait-specific-child-process-end
	// Implementing system() with fork, exec and wait.
	if (cmd_eq(cmd, ""))
		return;
	int pid = fork();
	if (pid < 0)
	{
		println("Error [systm()]: Unable to fork");
		return;
	}
	int status;
	if (pid == 0)
	{ // You can explore the diffrence of exec functions here:https://www.ibm.com/docs/en/aix/7.2?topic=e-exec-execl-execle-execlp-execv-execve-execvp-exect-fexecve-subroutine
		char *args = NULL;
		split_cmd(cmd, &args); // Check if any arguments, split cmd and argument if so.
		if (args == NULL || args == cmd)
			execlp(cmd, "", NULL); // No arguments
		else
		{
			char *a[] = { cmd, args, NULL }; // execlp doesn't fit
            		execvp(a[0], a);
		}
		exit(0); // Exit, we don't want our subprocess to continue in the program.
	}
	else do
	{
		sleep(0.001);
		if ((pid = waitpid(pid, &status, WNOHANG)) == -1)
		{
			println("Error while using wait()");
		}
	} while (pid == 0);
}
