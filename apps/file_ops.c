/**
 * file_ops.c - File Operations Tool
 * Copy, move, delete files with progress tracking
 */

#include <stdio.h>
#include <unistd.h>

/**
 * main - File Operations Tool entry point
 *
 * Provides file manipulation operations with progress display.
 *
 * Returns: 0 on success
 */
int main(void) {
    printf("\n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("                  FILE OPERATIONS TOOL                      \n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("  PID: %d\n", getpid());
    printf("  Status: Running as separate process\n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("\n");

    printf("Available Operations:\n");
    printf("  1. Copy files\n");
    printf("  2. Move files\n");
    printf("  3. Delete files\n");
    printf("  4. Batch operations\n");
    printf("\n");

    printf("Recent Operations:\n");
    printf("─────────────────────────────────────────────────────────────\n");
    printf("  [COPY]   report.pdf → /Documents/      [Complete] 100%%\n");
    printf("  [MOVE]   image.jpg → /Pictures/        [Complete] 100%%\n");
    printf("  [DELETE] temp.txt                      [Complete]\n");
    printf("─────────────────────────────────────────────────────────────\n");
    printf("\n");

    printf("Simulating file copy operation...\n");
    for (int i = 0; i <= 100; i += 20) {
        printf("Progress: [");
        for (int j = 0; j < i/5; j++) printf("█");
        for (int j = i/5; j < 20; j++) printf("░");
        printf("] %d%%\r", i);
        fflush(stdout);
        sleep(1);
    }
    printf("\nOperation completed!\n");
    printf("\n");

    printf("Press Enter to close...");
    getchar();

    return 0;
}
