/**
 * settings.c - Settings Panel Application
 * System configuration interface
 */

#include <stdio.h>
#include <unistd.h>

/**
 * main - Settings Panel entry point
 *
 * Displays and allows modification of system settings.
 *
 * Returns: 0 on success
 */
int main(void) {
    printf("\n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("                    SYSTEM SETTINGS                         \n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("  PID: %d\n", getpid());
    printf("  Status: Running as separate process\n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("\n");

    printf("Current System Configuration:\n");
    printf("─────────────────────────────────────────────────────────────\n");
    printf("  OS Name:           MiniOS\n");
    printf("  Version:           2.0\n");
    printf("  RAM:               2048 MB\n");
    printf("  HDD:               256 GB\n");
    printf("  CPU Cores:         8\n");
    printf("  Time Quantum:      100 ms\n");
    printf("  Max Processes:     64\n");
    printf("─────────────────────────────────────────────────────────────\n");
    printf("\n");

    printf("Press Enter to close...");
    getchar();

    return 0;
}
