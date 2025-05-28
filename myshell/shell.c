
#include "shell.h"

// Tableau global pour les commandes intégrées
builtin_t Global_builtin[MAX_BUILTINS];

// Fonction wrapper pour fork
pid_t cell_fork() {
    pid_t pid = fork();
    if (pid < 0) {
        p(RED "cell_fork: fork failed\n" RESET);
        exit(FAILURE);
    }
    return pid;
}

// Fonction wrapper pour execvp
int cell_execvp(char *file, char **args) {
    if (file == NULL || args == NULL) {
        p(RED "cell_execvp: invalid arguments\n" RESET);
        exit(FAILURE);
    }
    execvp(file, args);
    p(RED "cell_execvp: %s: command not found\n" RESET, file);
    exit(FAILURE);
}

// Fonction pour lancer une commande externe
void cell_launch(char **args) {
    pid_t pid;
    int status;

    pid = cell_fork();
    if (pid == 0) {
        // Processus enfant
        cell_execvp(args[0], args);
    } else if (pid > 0) {
        // Processus parent
        waitpid(pid, &status, WUNTRACED);
    }
}

// Fonction intégrée : exit
int cell_exit(char **args) {
    exit(SUCCESS);
    return 0;
}

// Fonction intégrée : env
int cell_env(char **args) {
    extern char **environ;
    if (environ == NULL) {
        p(RED "cell_env: environment variables not available\n" RESET);
        return FAILURE;
    }
    for (int i = 0; environ[i] != NULL; i++) {
        p("%s\n", environ[i]);
    }
    return SUCCESS;
}

// Fonction intégrée : cd
int cell_cd(char **args) {
    if (args[1] == NULL) {
        p(RED "cell_cd: expected argument (e.g., cd /path/to/directory)\n" RESET);
        return FAILURE;
    } else {
        if (chdir(args[1]) != 0) {
            p(RED "cell_cd: %s: directory not found\n" RESET, args[1]);
            return FAILURE;
        }
    }
    return SUCCESS;
}


char * cell_readline ( void ) { char prompt[ BUFFER_SIZE ];
if ( get_current_working_directory ( cwd , sizeof( cwd ))) { snprintf( prompt , sizeof( prompt), GREEN "% s \ U 0001 F919
$ " RESET , cwd );
} else {
snprintf( prompt , sizeof( prompt), GREEN " shell $ " RESET );
}
char * buffer = readline ( prompt); if ( buffer && * buffer) {
add_history ( buffer);
}
return buffer;
}




















// Fonction pour lire une ligne
char *cell_readline(void) {
    char *buffer = NULL;
    size_t buffer_size = 0;
    ssize_t chars_read = getline(&buffer, &buffer_size, stdin);

    if (chars_read == -1) {
        if (feof(stdin)) {
            p("End of file\n");
            free(buffer);
            return NULL;
        } else {
            p(RED "Getline failed\n" RESET);
            free(buffer);
            return NULL;
        }
    }
    return buffer;
}

// Fonction pour tokeniser une ligne
char **shell_split_line(char *line) {
    int buffer_size = BUFFER_SIZE;
    int position = 0;
    char **tokens = malloc(buffer_size * sizeof(char*));
    char *token;

    if (!tokens) {
        p(RED "shell_split_line: allocation error\n" RESET);
        exit(FAILURE);
    }

    token = strtok(line, DELIMITERS);
    while (token != NULL) {
        tokens[position] = token;
        position++;

        if (position >= buffer_size) {
            buffer_size *= 2;
            tokens = realloc(tokens, buffer_size * sizeof(char*));
            if (!tokens) {
                p(RED "shell_split_line: reallocation error\n" RESET);
                exit(FAILURE);
            }
        }
        token = strtok(NULL, DELIMITERS);
    }
    tokens[position] = NULL;
    return tokens;
}

// Fonction pour exécuter une commande
int shell_execute(char **args) {
    if (args[0] == NULL) {
        return SUCCESS; // Commande vide
    }

    // Vérifier si c'est une commande intégrée
    for (int i = 0; Global_builtin[i].name != NULL; i++) {
        if (strcmp(args[0], Global_builtin[i].name) == 0) {
            return (*Global_builtin[i].func)(args);
        }
    }

    // Si ce n'est pas une commande intégrée, lancer une commande externe
    cell_launch(args);
    return SUCCESS;
}

// Fonction pour obtenir le répertoire courant
char *get_current_working_directory(char *buffer, size_t size) {
    if (getcwd(buffer, size) == NULL) {
        p(RED "getcwd failed\n" RESET);
        return NULL;
    }
    return buffer;
}

// Fonction principale
int main() {
    // Initialiser les commandes intégrées
    Global_builtin[0].name = "exit";
    Global_builtin[0].func = cell_exit;
    Global_builtin[1].name = "env";
    Global_builtin[1].func = cell_env;
    Global_builtin[2].name = "cd";
    Global_builtin[2].func = cell_cd;
    Global_builtin[3].name = NULL; // Sentinelle

    // Bannière de démarrage
    p("   ___ ___ ._____________    _____   \n");
    p("  /   |   \\|   \\______   \\  /  _  \\  \n");
    p(" /    ~    \\   ||    |  _/ /  /_\\  \\ \n");
    p(" \\    Y    /   ||    |   \\/    |    \\\n");
    p("  \\___|_  /|___||______  /\\____|__  /\n");
    p("       \\/             \\/         \\/ \n");
    p("         Hiba's Shell               \n");
    p("                                    \n");

    char *line;
    char **args;
    char cwd[BUFFER_SIZE];

    // Boucle REPL
    while (1) {
        // Afficher l'invite personnalisée
        if (get_current_working_directory(cwd, sizeof(cwd))) {
            p(GREEN "%s \U0001F919 $ " RESET, cwd);
        } else {
            p(GREEN "shell $ " RESET);
        }

        // Lire l'entrée utilisateur
        line = cell_readline();
        if (line == NULL) {
            break; // Fin de l'entrée
        }

        // Tokeniser la ligne
        args = shell_split_line(line);

        // Exécuter la commande
        shell_execute(args);

        // Libérer la mémoire
        free(line);
        free(args);
    }

    return SUCCESS;
}
