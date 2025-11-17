/**
 * terminal.c - Terminal Emulator Application
 * Functional command-line interface with fork/exec for real command execution
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_ARGS 64
#define MAX_INPUT 1024

char current_dir[1024];

/**
 * parse_command - Parse command line into arguments
 */
int parse_command(char* input, char** args) {
    int argc = 0;
    char* token = strtok(input, " \t\n");

    while (token != NULL && argc < MAX_ARGS - 1) {
        args[argc++] = token;
        token = strtok(NULL, " \t\n");
    }

    args[argc] = NULL;
    return argc;
}

/**
 * builtin_cd - Change directory
 */
int builtin_cd(char** args) {
    if (args[1] == NULL) {
        fprintf(stderr, "cd: missing argument\n");
    } else {
        if (chdir(args[1]) != 0) {
            perror("cd");
        } else {
            getcwd(current_dir, sizeof(current_dir));
        }
    }
    return 1;
}

/**
 * builtin_pwd - Print working directory
 */
int builtin_pwd(char** args) {
    (void)args;  // Unused
    if (getcwd(current_dir, sizeof(current_dir)) != NULL) {
        printf("%s\n", current_dir);
    } else {
        perror("pwd");
    }
    return 1;
}

/**
 * builtin_help - Print help information
 */
int builtin_help(char** args) {
    (void)args;  // Unused
    printf("\nMiniOS Terminal v2.0\n");
    printf("═══════════════════════════════════════════════════════════\n");
    printf("Built-in Commands:\n");
    printf("  cd <dir>    - Change directory\n");
    printf("  pwd         - Print working directory\n");
    printf("  help        - Show this help\n");
    printf("  exit        - Exit terminal\n");
    printf("\nAny other command will be executed via fork/exec.\n");
    printf("Try: ls, ps, whoami, date, echo, cat, etc.\n");
    printf("═══════════════════════════════════════════════════════════\n\n");
    return 1;
}

/**
 * builtin_exit - Exit shell
 */
int builtin_exit(char** args) {
    (void)args;  // Unused
    return 0;
}

/**
 * execute_builtin - Check and execute built-in commands
 */
int execute_builtin(char** args) {
    if (args[0] == NULL) {
        return 1;  // Empty command
    }

    if (strcmp(args[0], "cd") == 0) {
        return builtin_cd(args);
    } else if (strcmp(args[0], "pwd") == 0) {
        return builtin_pwd(args);
    } else if (strcmp(args[0], "help") == 0) {
        return builtin_help(args);
    } else if (strcmp(args[0], "exit") == 0) {
        return builtin_exit(args);
    }

    return -1;  // Not a builtin
}

/**
 * execute_command - Fork and execute external command
 */
int execute_command(char** args) {
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        // Child process - execute command
        if (execvp(args[0], args) == -1) {
            fprintf(stderr, "terminal: command not found: %s\n", args[0]);
        }
        exit(EXIT_FAILURE);
    } else {
        // Parent process - wait for child
        int status;
        waitpid(pid, &status, 0);
    }

    return 1;
}

/**
 * main - Terminal Emulator entry point
 *
 * Functional shell with built-in commands and fork/exec
 * for external command execution.
 *
 * Returns: 0 on success
 */
int main(void) {
    char input[MAX_INPUT];
    char* args[MAX_ARGS];
    int status = 1;

    // Get initial working directory
    getcwd(current_dir, sizeof(current_dir));

    printf("\n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("                   TERMINAL EMULATOR                        \n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("  PID: %d\n", getpid());
    printf("  Status: Running as REAL separate process\n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("\n");

    printf("MiniOS Terminal v2.0\n");
    printf("Type 'help' for available commands, 'exit' to quit\n");
    printf("Built-in commands: cd, pwd, help, exit\n");
    printf("All other commands are executed via fork/exec\n");
    printf("\n");

    while (status) {
        // Print prompt with current directory
        char* last_dir = strrchr(current_dir, '/');
        if (last_dir != NULL) {
            printf("[%s]$ ", last_dir + 1);
        } else {
            printf("$ ");
        }

        fflush(stdout);

        // Read command
        if (fgets(input, MAX_INPUT, stdin) == NULL) {
            printf("\n");
            break;
        }

        // Skip empty lines
        if (input[0] == '\n') {
            continue;
        }

        // Parse command
        int argc = parse_command(input, args);

        if (argc == 0) {
            continue;
        }

        // Try to execute as builtin first
        int builtin_result = execute_builtin(args);

        if (builtin_result == -1) {
            // Not a builtin, execute as external command
            status = execute_command(args);
        } else {
            status = builtin_result;
        }
    }

    printf("\nTerminal closing...\n");
    return 0;
}
