void print(char *str);
void println(char *str);
void println_from(char *str, int position); // For simple syntax printing
void get_command(char **cmd, int *cmd_len);
int cmd_eq(char *actual, char *expected); // Command equals (strcmp alternative)
int cmd_sw(char *actual, char *expected); // Command starts with
void double_quote(char *cmd, char **first, char **second, int block_overload); // 2 arguments parsing while handling quotes
void split_cmd(char *cmd, char **args);
