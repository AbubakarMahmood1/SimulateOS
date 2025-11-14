/**
 * terminal.c - Terminal Emulator Application
 * Command-line interface
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>

/**
 * main - Terminal Emulator entry point
 *
 * Provides a simple command-line interface.
 *
 * Returns: 0 on success
 */
int main(void) {
    char command[256];

    printf("\n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("                   TERMINAL EMULATOR                        \n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("  PID: %d\n", getpid());
    printf("  Status: Running as separate process\n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("\n");

    printf("MiniOS Terminal v2.0\n");
    printf("Type 'help' for available commands, 'exit' to quit\n");
    printf("\n");

    while (1) {
        printf("$ ");
        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = 0;  // Remove newline

        if (strcmp(command, "exit") == 0) {
            break;
        } else if (strcmp(command, "help") == 0) {
            printf("Available commands: ls, pwd, help, exit\n");
        } else if (strcmp(command, "ls") == 0) {
            printf("documents/  downloads/  pictures/  notes.txt\n");
        } else if (strcmp(command, "pwd") == 0) {
            printf("/home/user\n");
        } else if (strlen(command) > 0) {
            printf("Command not found: %s\n", command);
        }
    }

    printf("Terminal closing...\n");
    return 0;
}
