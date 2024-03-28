#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_LINE 1024
#define MAX_ARGS 128
#define MAX_COMM 100
#define DELIM " \t\r\n\a"

char history[MAX_COMM][MAX_LINE];
int history_index = 0;

// prototypes
void add_to_history(const char *cmd);
void execute_with_pipes(char *cmd);
int tokenize_command(char *cmd, char **argv);
void run_builtin_command(char **args);
void show_history();
void clear_history();

int main() {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    while (1) {
        printf("sish> ");
        nread = getline(&line, &len, stdin);
        if (nread == -1) {
            if (feof(stdin)) {
                break; // exit on EOF
            } else {
                perror("getline");
                continue; // continue on error
            }
        }

        line[strcspn(line, "\n")] = 0; // remove newline
        if (line[0] == '\0') continue; // skip empty lines

        add_to_history(line);
        execute_with_pipes(line);
    }

    free(line);
    return EXIT_SUCCESS;
}

void execute_command_from_history(int offset) {
    if (offset < 0 || offset >= history_index) {
        printf("Invalid history offset: %d\n", offset);
        return;
    }
    char cmd[MAX_LINE];
    strncpy(cmd, history[offset], MAX_LINE);
    printf("Re-executing: %s\n", cmd);
    execute_with_pipes(cmd);
}


void execute_with_pipes(char *cmd) {
    int num_cmds = 0;
    char *commands[MAX_COMM];
    char *token = strtok(cmd, "|");
    while (token != NULL) {
        commands[num_cmds++] = token;
        token = strtok(NULL, "|");
    }

    int i = 0;
    int fd[2], in_fd = 0;
    while (i < num_cmds) {
        char *argv[MAX_ARGS];
        tokenize_command(commands[i], argv);

        // Run built in commands directly 
        if (i == 0 && num_cmds == 1 && (strcmp(argv[0], "cd") == 0 || strcmp(argv[0], "history") == 0)) {
            run_builtin_command(argv);
            return;
        }

        pipe(fd);

        if (fork() == 0) {
            dup2(in_fd, 0); // redirect input to stdin
            if (i < num_cmds - 1) {
                dup2(fd[1], 1); // redirect stdout to output of the pipe
            }
            close(fd[0]);
            execvp(argv[0], argv);
            perror("execvp");
            exit(EXIT_FAILURE);
        } else {
            wait(NULL); 
            close(fd[1]);
            in_fd = fd[0]; // save input for the next command
            i++;
        }
    }
}

int tokenize_command(char *cmd, char **argv) {
    int count = 0;
    char *token = strtok(cmd, DELIM);
    while (token != NULL) {
        argv[count++] = token;
        token = strtok(NULL, DELIM);
    }
    argv[count] = NULL;
    return count;
}

void run_builtin_command(char **args) {
    if (strcmp(args[0], "cd") == 0) {
        if (args[1] == NULL) {
            fprintf(stderr, "sish: expected argument to \"cd\"\n");
        } else {
            if (chdir(args[1]) != 0) {
                perror("sish");
            }
        }
    } else if (strcmp(args[0], "history") == 0) {
        if (args[1] != NULL) {
            // execute a command from history
            if (strcmp(args[1], "-c") == 0) {
                clear_history();
            } else {
                int offset = atoi(args[1]);
                if (offset >= 0 && offset < history_index) {
                    execute_command_from_history(offset);
                } else {
                    printf("Invalid offset: %s\n", args[1]);
                }
            }
        } else {
            show_history();
        }
    }
}


void add_to_history(const char *cmd) {
    strncpy(history[history_index % MAX_COMM], cmd, MAX_LINE - 1);
    history[history_index % MAX_COMM][MAX_LINE - 1] = '\0'; // null termination
    history_index++;
}

void show_history() {
    for (int i = 0; i < history_index; i++) {
        printf("%d %s\n", i, history[i]);
    }
}

void clear_history() {
    memset(history, 0, sizeof(history));
    history_index = 0;
}

