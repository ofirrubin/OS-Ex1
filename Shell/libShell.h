void print_cwd(void);
void cd(char *path);
void dir(void);
void clear(void);
void systm(char *command); // System by fork, exec, wait.
void delete_file(char *fname);
void copy(char *command); // Also parses the command.
