#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <ncurses.h> // Pour les couleurs dans le terminal

#define p(...) printf(__VA_ARGS__)
#define RED "\0330;31m"
#define RESET "\0330m"
#define GREEN "\0330;32m"

#define SUCCESS 0
#define FAILURE 1
#define BUFFER_SIZE 1024
#define DELIMITERS " \t\r\n\a"
#define MAX_BUILTINS 10

// Structure pour les commandes intégrées
typedef struct {
    const char *name;
    int (*func)(char **args);
} builtin_t;

// Prototypes des fonctions
char *cell_readline(void);
char **shell_split_line(char *line);
int shell_execute(char **args);
void cell_launch(char **args);
pid_t cell_fork(void);
int cell_execvp(char *file, char **args);
char *get_current_working_directory(char *buffer, size_t size);
int cell_exit(char **args);
int cell_env(char **args);
int cell_cd(char **args);
// Déclaration des fonctions intégrées
extern builtin_t Global_builtin[MAX_BUILTINS];

#endif