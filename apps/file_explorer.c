/**
 * file_explorer.c - File Explorer Application
 * Browse and manage files
 */

#include <stdio.h>
#include <unistd.h>

/**
 * main - File Explorer entry point
 *
 * Simulates a file browser for navigating directories.
 *
 * Returns: 0 on success
 */
int main(void) {
    printf("\n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("                    FILE EXPLORER                           \n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("  PID: %d\n", getpid());
    printf("  Status: Running as separate process\n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("\n");

    printf("Current Directory: /home/user/\n");
    printf("\n");
    printf("Files and Folders:\n");
    printf("─────────────────────────────────────────────────────────────\n");
    printf("  [DIR]  Documents\n");
    printf("  [DIR]  Downloads\n");
    printf("  [DIR]  Pictures\n");
    printf("  [FILE] notes.txt (2.5 KB)\n");
    printf("  [FILE] report.pdf (1.2 MB)\n");
    printf("  [FILE] presentation.pptx (5.8 MB)\n");
    printf("─────────────────────────────────────────────────────────────\n");
    printf("\n");

    printf("Press Enter to close...");
    getchar();

    return 0;
}
