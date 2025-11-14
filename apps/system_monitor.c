/**
 * system_monitor.c - System Monitor Application
 * Displays real-time system resource usage
 */

#include <stdio.h>
#include <unistd.h>
#include <time.h>

/**
 * main - System Monitor entry point
 *
 * Simulates a system monitor showing resource usage.
 * Runs as a separate process demonstrating process isolation.
 *
 * Returns: 0 on success
 */
int main(void) {
    printf("\n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("                    SYSTEM MONITOR                          \n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("  PID: %d\n", getpid());
    printf("  Status: Running as separate process\n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("\n");

    printf("Monitoring system resources...\n");
    printf("\n");

    for (int i = 0; i < 5; i++) {
        time_t now = time(NULL);
        printf("[%s] CPU: %d%%  |  RAM: %d%%  |  Processes: %d\n",
               ctime(&now), 20 + (i * 5), 30 + (i * 3), 10 + i);
        sleep(2);
    }

    printf("\nSystem Monitor shutting down...\n");
    return 0;
}
