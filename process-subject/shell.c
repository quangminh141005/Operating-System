#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>

#define MAXCOMMAND 1024
#define MAXARGS 100

void exec_simple(char *cmd);
void exec_pipe(char *cmd1, char *cmd2);
void parse_command(char *cmd, char ** args);

int main(void) {
    while(1) {
        char input[MAXCOMMAND];
        printf("Shell> ");
        fgets(input, MAXCOMMAND, stdin);
        input[strcspn(input, "\n")] = 0;

        char *cmd1 = strtok(input, "|");
        char *cmd2 = strtok(NULL, "|");

        if (cmd2) {
            exec_pipe(cmd1, cmd2);
        } else if (!strcmp(cmd1, "exit")) {
            printf("Quitting programn...\n");
            break;
        } else {
            exec_simple(cmd1);
        }
    }

    return 0;
}

void parse_command(char *cmd, char ** args) {
    int i = 0;
    char *token = strtok(cmd, " ");
    while (token != NULL) {
        args[i++] = token;
        token = strtok(NULL, " ");
    }
    args[i] = NULL;
}

// execute simple function
void exec_simple(char *cmd) {
    pid_t pid = fork();
    if (pid == 0) {
        char *args[MAXARGS];
        parse_command(cmd, args);
        execvp(args[0], args);
        perror("Error executing simple cmd\n");

    } else if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);
    } else {
        perror("Error creating child process\n");
    }
}

// Execute pipe function (2 pipe)
void exec_pipe(char *cmd1, char *cmd2) {
    int fd[2];
    pipe(fd);
    pid_t pid1 = fork();
    if (pid1 == 0) {
        dup2(fd[1], STDOUT_FILENO); // STDOUT_FILENO = 1
        close(fd[0]);
        char *args1[MAXARGS];
        parse_command(cmd1, args1);
        execvp(args1[0], args1);
        perror("Error creating chile pipe input process\n");
        exit(EXIT_FAILURE);
    }

    pid_t pid2 = fork(); // Creating child process for handle output
    if (pid2 == 0) {
        dup2(fd[0], STDIN_FILENO);
        close(fd[1]);
        char *args2[MAXARGS];
        parse_command(cmd2, args2);
        execvp(args2[0], args2);    
        perror("error creating child pipe output process\n");
        exit(EXIT_FAILURE);
    }

    close(fd[0]);
    close(fd[1]);
    int status;
    waitpid(pid1, &status, 0);
    waitpid(pid2, &status, 0);
}

