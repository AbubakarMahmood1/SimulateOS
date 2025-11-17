/**
 * clock.c - Clock & Timer Application
 * Real-time clock with world timezones, alarms, and timer functionality
 */

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

#define MAX_ALARMS 10

typedef struct {
    int hour;
    int minute;
    char label[50];
    int active;
} Alarm;

Alarm alarms[MAX_ALARMS];
int alarm_count = 0;
volatile sig_atomic_t running = 1;

/**
 * signal_handler - Handle interrupt signals
 */
void signal_handler(int signum) {
    (void)signum;  // Unused parameter
    running = 0;
}

/**
 * add_alarm - Add a new alarm
 */
void add_alarm(int hour, int minute, const char* label) {
    if (alarm_count >= MAX_ALARMS) {
        printf("Maximum alarm limit reached!\n");
        return;
    }

    alarms[alarm_count].hour = hour;
    alarms[alarm_count].minute = minute;
    strncpy(alarms[alarm_count].label, label, sizeof(alarms[alarm_count].label) - 1);
    alarms[alarm_count].active = 1;
    alarm_count++;

    printf("Alarm set for %02d:%02d - %s\n", hour, minute, label);
}

/**
 * check_alarms - Check if any alarm should trigger
 */
void check_alarms(int current_hour, int current_minute) {
    for (int i = 0; i < alarm_count; i++) {
        if (alarms[i].active &&
            alarms[i].hour == current_hour &&
            alarms[i].minute == current_minute) {
            printf("\n🔔 ALARM! %s\n", alarms[i].label);
            alarms[i].active = 0;  // Deactivate after triggering
        }
    }
}

/**
 * print_world_clocks - Display time in multiple timezones
 */
void print_world_clocks(struct tm* tm_info) {
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
    printf("  Sydney (AEDT):     %02d:%02d:%02d\n",
           (tm_info->tm_hour + 11) % 24, tm_info->tm_min, tm_info->tm_sec);
}

/**
 * print_alarms - Display all active alarms
 */
void print_alarms(void) {
    printf("\nActive Alarms:\n");
    printf("─────────────────────────────────────────────────────────────\n");

    if (alarm_count == 0) {
        printf("  No alarms set\n");
    } else {
        for (int i = 0; i < alarm_count; i++) {
            if (alarms[i].active) {
                printf("  %d. %02d:%02d - %s\n",
                       i + 1, alarms[i].hour, alarms[i].minute, alarms[i].label);
            } else {
                printf("  %d. %02d:%02d - %s (triggered)\n",
                       i + 1, alarms[i].hour, alarms[i].minute, alarms[i].label);
            }
        }
    }
    printf("─────────────────────────────────────────────────────────────\n");
}

/**
 * run_timer - Run a countdown timer
 */
void run_timer(int seconds) {
    printf("\nTimer started for %d seconds...\n", seconds);

    for (int i = seconds; i > 0; i--) {
        printf("\rTime remaining: %02d:%02d", i / 60, i % 60);
        fflush(stdout);
        sleep(1);
    }

    printf("\n\n🔔 TIMER FINISHED!\a\n\n");
}

/**
 * print_help - Display help information
 */
void print_help(void) {
    printf("\nAvailable Commands:\n");
    printf("  clock    - Show live clock (Ctrl+C to stop)\n");
    printf("  world    - Show world timezones\n");
    printf("  alarm    - Set a new alarm (e.g., alarm 14 30 Meeting)\n");
    printf("  list     - List all alarms\n");
    printf("  timer    - Start countdown timer (e.g., timer 60)\n");
    printf("  help     - Show this help\n");
    printf("  exit     - Exit application\n");
    printf("\n");
}

/**
 * run_live_clock - Display continuously updating clock
 */
void run_live_clock(void) {
    time_t now;
    struct tm* tm_info;
    char time_str[100];
    char date_str[100];
    int last_minute = -1;

    printf("\nLive Clock (Press Ctrl+C to stop)\n");
    printf("═══════════════════════════════════════════════════════════\n\n");

    running = 1;
    signal(SIGINT, signal_handler);

    while (running) {
        now = time(NULL);
        tm_info = localtime(&now);

        strftime(time_str, sizeof(time_str), "%H:%M:%S", tm_info);
        strftime(date_str, sizeof(date_str), "%A, %B %d, %Y", tm_info);

        // Clear previous line and print new time
        printf("\r  🕐 %s  |  %s", time_str, date_str);
        fflush(stdout);

        // Check alarms once per minute
        if (tm_info->tm_min != last_minute) {
            check_alarms(tm_info->tm_hour, tm_info->tm_min);
            last_minute = tm_info->tm_min;
        }

        sleep(1);
    }

    printf("\n\n");
    signal(SIGINT, SIG_DFL);  // Restore default handler
}

/**
 * main - Clock & Timer entry point
 *
 * Full-featured clock with real-time display, world timezones,
 * alarms, and countdown timer functionality.
 *
 * Returns: 0 on success
 */
int main(void) {
    char input[256];
    char command[50];
    int hour, minute;
    char label[200];
    int timer_seconds;

    printf("\n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("                    CLOCK & TIMER                           \n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("  PID: %d\n", getpid());
    printf("  Status: Running as REAL separate process\n");
    printf("  Priority: LOW (System Task)\n");
    printf("════════════════════════════════════════════════════════════\n");

    // Add some default alarms
    add_alarm(7, 0, "Wake up");
    add_alarm(9, 0, "Class starts");
    add_alarm(18, 0, "Evening workout");

    print_help();

    while (1) {
        printf("\nclock> ");
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }

        // Remove newline
        input[strcspn(input, "\n")] = 0;

        // Skip empty input
        if (strlen(input) == 0) {
            continue;
        }

        // Parse command
        sscanf(input, "%s", command);

        if (strcmp(command, "exit") == 0 || strcmp(command, "quit") == 0) {
            printf("\nClock shutting down...\n");
            break;
        }

        if (strcmp(command, "help") == 0) {
            print_help();
            continue;
        }

        if (strcmp(command, "clock") == 0) {
            run_live_clock();
            continue;
        }

        if (strcmp(command, "world") == 0) {
            time_t now = time(NULL);
            struct tm* tm_info = localtime(&now);
            printf("\n");
            print_world_clocks(tm_info);
            printf("\n");
            continue;
        }

        if (strcmp(command, "list") == 0) {
            print_alarms();
            continue;
        }

        if (strncmp(input, "alarm ", 6) == 0) {
            // Parse: alarm HH MM Label
            if (sscanf(input, "alarm %d %d %[^\n]", &hour, &minute, label) >= 2) {
                if (hour < 0 || hour > 23 || minute < 0 || minute > 59) {
                    printf("Invalid time! Use 24-hour format (HH MM)\n");
                } else {
                    if (sscanf(input, "alarm %d %d %[^\n]", &hour, &minute, label) < 3) {
                        strcpy(label, "Alarm");
                    }
                    add_alarm(hour, minute, label);
                }
            } else {
                printf("Usage: alarm HH MM [label]\n");
                printf("Example: alarm 14 30 Meeting\n");
            }
            continue;
        }

        if (strncmp(input, "timer ", 6) == 0) {
            if (sscanf(input, "timer %d", &timer_seconds) == 1) {
                if (timer_seconds <= 0 || timer_seconds > 86400) {
                    printf("Invalid timer duration! Use 1-86400 seconds\n");
                } else {
                    run_timer(timer_seconds);
                }
            } else {
                printf("Usage: timer SECONDS\n");
                printf("Example: timer 60\n");
            }
            continue;
        }

        printf("Unknown command '%s'. Type 'help' for available commands.\n", command);
    }

    return 0;
}
