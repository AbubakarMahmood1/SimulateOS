/**
 * calendar.c - Calendar Application
 * Date and event management
 */

#include <stdio.h>
#include <time.h>
#include <unistd.h>

/**
 * main - Calendar entry point
 *
 * Displays current date and allows event management.
 *
 * Returns: 0 on success
 */
int main(void) {
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    char date_str[100];

    strftime(date_str, sizeof(date_str), "%B %d, %Y", tm_info);

    printf("\n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("                        CALENDAR                            \n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("  PID: %d\n", getpid());
    printf("  Status: Running as separate process\n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("\n");

    printf("Today: %s\n", date_str);
    printf("\n");

    printf("Upcoming Events:\n");
    printf("─────────────────────────────────────────────────────────────\n");
    printf("  Nov 20, 2025 - Team Meeting (10:00 AM)\n");
    printf("  Nov 22, 2025 - Project Deadline (5:00 PM)\n");
    printf("  Nov 25, 2025 - OS Lab Presentation (2:00 PM)\n");
    printf("  Nov 30, 2025 - Final Exam (9:00 AM)\n");
    printf("─────────────────────────────────────────────────────────────\n");
    printf("\n");

    printf("Press Enter to close...");
    getchar();

    return 0;
}
