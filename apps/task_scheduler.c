/**
 * task_scheduler.c - Task Scheduler Application
 * Schedule process execution
 */

#include <stdio.h>
#include <unistd.h>

/**
 * main - Task Scheduler entry point
 *
 * Allows scheduling of tasks for future execution.
 *
 * Returns: 0 on success
 */
int main(void) {
    printf("\n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("                     TASK SCHEDULER                         \n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("  PID: %d\n", getpid());
    printf("  Status: Running as separate process\n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("\n");

    printf("Scheduled Tasks:\n");
    printf("─────────────────────────────────────────────────────────────\n");
    printf("  ID  | Task Name          | Schedule       | Status\n");
    printf("─────────────────────────────────────────────────────────────\n");
    printf("  001 | Backup System      | Daily 2:00 AM  | Active\n");
    printf("  002 | Update Software    | Weekly Mon     | Active\n");
    printf("  003 | Clean Temp Files   | Daily 3:00 AM  | Active\n");
    printf("  004 | Generate Reports   | Monthly 1st    | Pending\n");
    printf("─────────────────────────────────────────────────────────────\n");
    printf("\n");

    printf("Next scheduled task: Backup System (in 4 hours)\n");
    printf("\n");

    printf("Press Enter to close...");
    getchar();

    return 0;
}
