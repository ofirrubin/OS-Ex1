#include "shellParsing.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void get_command(char **cmd, int *cmd_len)
{
	int sz = sizeof(char) * 512;
	char *c = calloc(1, sz); // free(cmd) has to be called
	*cmd = c;
	if (!c){
		printf("shell: Error allocating for command input\n");
		exit(0);
	}
	*cmd_len = sz;
	char current = getchar(); // Get input from the user
	int position = 0;
	while (current != EOF && current != '\n'){
		c[position] = current;
		position += 1;
		current = getchar();
	}
	c[position + 1] = 0; // Make iterable
	*cmd_len = position;
}

void print(char *cmd){
	printf("shell: ");
	int pos = 0;
	while (cmd[pos] != 0){
		printf("%c", cmd[pos++]);
	}
}

void println(char *cmd){
	print(cmd);
	printf("\n");
}

void println_from(char *cmd, int start_at){
	println(cmd + start_at);
}

int cmd_cmp(char *actual, char *expected, int starts_with){ // Assuming <expected> is lowered case.
	int pos = 0;
	int d = 'A' - 'a';
	while (actual[pos] != 0){
		if (expected[pos] == 0 || !(actual[pos] == expected[pos] || expected[pos] + d == actual[pos]))
			return starts_with && expected[pos] == 0;
		pos += 1;
	}
	return expected[pos] == 0 && actual[pos] == 0;
}

int cmd_eq(char *cmd, char *cmp){ // Command equals to
	return cmd_cmp(cmd, cmp, 0);
}

int cmd_sw(char *cmd, char *cmp){ // Command Starts with
	return cmd_cmp(cmd, cmp, 1);
}

int get_len(char *cmd) // get cmd length, untile there is 0
{
	int c = 0;
	char *p = cmd;
	while (p[c] != 0)
		c ++;
	return c;
}

int quote_parser(char *cmd, int *start_position, int *quote_len)
{
	int position = *start_position;
	
	int is_q = 0;
	int is_complete = 0;
	// Starting with source:
	while (cmd[position] != 0 && !is_complete){
		// On quote
		if (cmd[position] == '"')
		{
			if (is_q)
			{
				if (*quote_len < 1 || !(is_q == 0 || is_q == 1)){ // Illegal quote
					println("Error, invalid quote syntax; Make sure you use the follwoing form: '\"<argument>\"', argument can't be empty");
					return 0;
				}
				else // Quote closed
					is_complete = 1;
			}
			else{
				if (position != *start_position)  // A quote can be started only at the beginning of the string
				{
					println("Error, invalid quote syntax, quote can only be made at the beginning of the argument");
					return 0;
				}
				is_q ++; // Starting a quote
				*start_position = position + 1;
			}
		}
		else if (! is_q && (cmd[position] == ' ' || cmd[position] == '\n' || cmd[position + 1] == 0)){ // We are not in quote and we have white space / end of command.
			if (cmd[position + 1] == 0)
				*quote_len += 1;
			is_complete = 1;
		}
		else
			*quote_len += 1;
		position += 1;
	}
	return is_complete; // return if quote valid
}

void double_quote(char *cmd, char **first, char **second, int block_overload){
	int src_pos = 0;
	int src_len = 0;
	int dest_pos = 0;
	int dest_len = 0;
	
	if (quote_parser(cmd, &src_pos, &src_len) == 0){
		println("Synatx error (src): Please make sure you use quotes if you have space in your path, verify you have openning and closing quotes.");
		return;
	}
	dest_pos = src_pos + src_len;
	if (src_len < 1)
	{
		println("Error, You must set both source and dest to");
		return;
	}
	if (cmd[dest_pos] == '"') // Path has quote
	{
		cmd[dest_pos] = 0;
		dest_pos ++;
	}
	if (cmd[dest_pos] != ' ') // If len(arguments) < 2, show error.
	{
		println("Syntax error: You must divide source and dest by white space (' ')");
		return;
	}
	cmd[dest_pos++] = 0; // Split the string so we can use it later; Not having to pass both lengths.
	if (quote_parser(cmd, &dest_pos, &dest_len) == 0){
		println("Syntax error (dest): Make sure you have closing and starting quotes if using any");
		return;
	}
	if (dest_len < 1)
	{
		println("Error, You must set both source and dest");
		return;
	}
	if (block_overload && get_len(cmd + dest_pos + dest_len) > 1) // If it has characters at the end & set to block overloading => len(args) > 2
	{
		if (cmd[dest_pos + dest_len + 1] == ' ')
			println("Error, the command takes two arguments only.");
		else
			println("Syntax error; Seems like last command is chained. Unknown format");
		return;
	}
	cmd[dest_pos + dest_len] = 0;
	*first = cmd + src_pos;
	*second = cmd + dest_pos;
}


void split_cmd(char *cmd, char **args)
{
	int ptr = 0;
	while (!(!cmd[ptr] || cmd[ptr] == ' ' || cmd[ptr] == '\n' || cmd[ptr] == EOF))
	{
		ptr ++;
	}
	if (cmd[ptr] == 0) // No arguments
		return;
	cmd[ptr] = 0;
	while (cmd[ptr] == ' ') // Go to actual argument (skip blanks)
		ptr++;
	if (cmd[ptr + 1] == 0) // No actual argument
		return;
	*args = cmd + ptr + 1;
}
