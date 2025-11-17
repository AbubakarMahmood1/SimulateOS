/**
 * calendar.c - Calendar Application
 * Date display and simple event management
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define MAX_EVENTS 50

typedef struct {
    int day;
    int month;
    int year;
    char description[100];
} Event;

Event events[MAX_EVENTS];
int event_count = 0;

void print_month(int month, int year) {
    char *months[] = {"", "January", "February", "March", "April", "May", "June",
                      "July", "August", "September", "October", "November", "December"};
    int days_in_month[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    
    // Check for leap year
    if (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) {
        days_in_month[2] = 29;
    }
    
    printf("\n        %s %d\n", months[month], year);
    printf(" Sun Mon Tue Wed Thu Fri Sat\n");
    printf("────────────────────────────\n");
    
    // Calculate first day of month (simplified)
    struct tm time_struct = {0};
    time_struct.tm_year = year - 1900;
    time_struct.tm_mon = month - 1;
    time_struct.tm_mday = 1;
    mktime(&time_struct);
    int first_day = time_struct.tm_wday;
    
    // Print leading spaces
    for (int i = 0; i < first_day; i++) {
        printf("    ");
    }
    
    // Print days
    for (int day = 1; day <= days_in_month[month]; day++) {
        printf("%4d", day);
        if ((day + first_day) % 7 == 0) {
            printf("\n");
        }
    }
    printf("\n\n");
}

int main(void) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    int current_month = tm_info->tm_mon + 1;
    int current_year = tm_info->tm_year + 1900;
    int current_day = tm_info->tm_mday;
    
    printf("\n════════════════════════════════════════════════════════════\n");
    printf("                        CALENDAR                            \n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("  PID: %d\n", getpid());
    printf("  Status: Running as REAL separate process\n");
    printf("════════════════════════════════════════════════════════════\n");
    
    // Add some sample events
    events[event_count++] = (Event){20, 11, 2025, "Team Meeting"};
    events[event_count++] = (Event){25, 11, 2025, "OS Lab Presentation"};
    events[event_count++] = (Event){30, 11, 2025, "Final Exam"};
    
    char input[256];
    while (1) {
        print_month(current_month, current_year);
        
        printf("Today: %d/%d/%d\n\n", current_day, current_month, current_year);
        printf("Upcoming Events:\n");
        printf("─────────────────────────────────────────────────────────────\n");
        for (int i = 0; i < event_count; i++) {
            printf("  %02d/%02d/%d - %s\n", events[i].day, events[i].month, 
                   events[i].year, events[i].description);
        }
        printf("─────────────────────────────────────────────────────────────\n");
        
        printf("\nCommands: next, prev, today, exit\n");
        printf("calendar> ");
        fflush(stdout);
        
        if (fgets(input, sizeof(input), stdin) == NULL) break;
        input[strcspn(input, "\n")] = 0;
        
        if (strcmp(input, "exit") == 0) break;
        if (strcmp(input, "next") == 0) {
            current_month++;
            if (current_month > 12) {
                current_month = 1;
                current_year++;
            }
        } else if (strcmp(input, "prev") == 0) {
            current_month--;
            if (current_month < 1) {
                current_month = 12;
                current_year--;
            }
        } else if (strcmp(input, "today") == 0) {
            current_month = tm_info->tm_mon + 1;
            current_year = tm_info->tm_year + 1900;
        }
    }
    
    printf("\nCalendar closing...\n");
    return 0;
}
