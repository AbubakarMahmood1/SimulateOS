/**
 * process_manager.c - Process Manager Application
 * Lists and manages running processes
 */

#include <stdio.h>
#include <unistd.h>

/**
 * main - Process Manager entry point
 *
 * Displays running processes and allows process management.
 *
 * Returns: 0 on success
 */
int main(void) {
    printf("\n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("                   PROCESS MANAGER                          \n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("  PID: %d\n", getpid());
    printf("  Status: Running as separate process\n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("\n");

    printf("Running Processes:\n");
    printf("─────────────────────────────────────────────────────────────\n");
    printf(" PID  | Name               | State   | Priority | Memory\n");
    printf("─────────────────────────────────────────────────────────────\n");
    printf(" 1001 | System Monitor     | RUNNING | HIGH     | 64 MB\n");
    printf(" 1002 | Notepad+           | READY   | HIGH     | 64 MB\n");
    printf(" 1003 | Calculator         | RUNNING | HIGH     | 32 MB\n");
    printf(" 1004 | Music Player       | WAITING | MEDIUM   | 96 MB\n");
    printf("─────────────────────────────────────────────────────────────\n");
    printf("\n");

    printf("Press Enter to close...");
    getchar();

    return 0;
}
