/**
 * clock.c - Clock & Timer Application
 * Multiple timezone display with alarms
 */

#include <stdio.h>
#include <time.h>
#include <unistd.h>

/**
 * main - Clock & Timer entry point
 *
 * Displays current time in multiple timezones with alarm functionality.
 *
 * Returns: 0 on success
 */
int main(void) {
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    char time_str[100];

    printf("\n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("                    CLOCK & TIMER                           \n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("  PID: %d\n", getpid());
    printf("  Status: Running as separate process\n");
    printf("  Priority: LOW (System Task)\n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("\n");

    strftime(time_str, sizeof(time_str), "%H:%M:%S", tm_info);

    printf("Current Time:\n");
    printf("─────────────────────────────────────────────────────────────\n");
    printf("  Local Time:        %s\n", time_str);
    printf("─────────────────────────────────────────────────────────────\n");
    printf("\n");

    printf("World Clocks:\n");
    printf("─────────────────────────────────────────────────────────────\n");
    printf("  New York (EST):    %02d:%02d:%02d\n",
           (tm_info->tm_hour - 5 + 24) % 24, tm_info->tm_min, tm_info->tm_sec);
    printf("  London (GMT):      %02d:%02d:%02d\n",
           tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec);
    printf("  Tokyo (JST):       %02d:%02d:%02d\n",
           (tm_info->tm_hour + 9) % 24, tm_info->tm_min, tm_info->tm_sec);
    printf("  Dubai (GST):       %02d:%02d:%02d\n",
           (tm_info->tm_hour + 4) % 24, tm_info->tm_min, tm_info->tm_sec);
    printf("─────────────────────────────────────────────────────────────\n");
    printf("\n");

    printf("Active Alarms:\n");
    printf("─────────────────────────────────────────────────────────────\n");
    printf("  ⏰ 07:00 AM - Wake up\n");
    printf("  ⏰ 09:00 AM - Class starts\n");
    printf("  ⏰ 06:00 PM - Evening workout\n");
    printf("─────────────────────────────────────────────────────────────\n");
    printf("\n");

    printf("Live Clock (updating every second):\n");
    for (int i = 0; i < 5; i++) {
        now = time(NULL);
        tm_info = localtime(&now);
        strftime(time_str, sizeof(time_str), "%H:%M:%S", tm_info);
        printf("\r  🕐 %s", time_str);
        fflush(stdout);
        sleep(1);
    }
    printf("\n\n");

    printf("Press Enter to close...");
    getchar();

    return 0;
}
