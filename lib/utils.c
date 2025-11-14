/**
 * utils.c - Utility functions
 * Common helper functions used throughout the system
 */

#include "../include/kernel.h"
#include "../include/types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

/**
 * clear_screen - Clear the terminal screen
 *
 * Clears the terminal using ANSI escape codes.
 */
void clear_screen(void) {
    printf("\033[2J\033[H");
    fflush(stdout);
}

/**
 * sleep_ms - Sleep for specified milliseconds
 * @milliseconds: Time to sleep in milliseconds
 *
 * Sleeps for the specified number of milliseconds.
 */
void sleep_ms(int milliseconds) {
    usleep(milliseconds * 1000);
}

/**
 * print_system_status - Print current system status
 * @resources: System resources structure
 *
 * Displays current resource utilization and process count.
 */
void print_system_status(SystemResources* resources) {
    if (resources == NULL) {
        return;
    }

    pthread_mutex_lock(&resources->resource_lock);

    printf("\n");
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║                      SYSTEM STATUS                           ║\n");
    printf("╠══════════════════════════════════════════════════════════════╣\n");

    // RAM status
    int ram_used = resources->total_ram - resources->available_ram;
    int ram_percent = (ram_used * 100) / resources->total_ram;
    printf("║ RAM:   %5d / %5d MB  (%3d%% used)                      ║\n",
           ram_used, resources->total_ram, ram_percent);

    // HDD status
    int hdd_used = resources->total_hdd - resources->available_hdd;
    int hdd_percent = (hdd_used * 100) / resources->total_hdd;
    printf("║ HDD:   %5d / %5d MB  (%3d%% used)                      ║\n",
           hdd_used, resources->total_hdd, hdd_percent);

    // CPU status
    int cores_used = resources->total_cores - resources->available_cores;
    int cores_percent = (cores_used * 100) / resources->total_cores;
    printf("║ CPU:   %5d / %5d cores (%3d%% used)                     ║\n",
           cores_used, resources->total_cores, cores_percent);

    // Process count
    printf("║                                                              ║\n");
    printf("║ Active Processes: %3d / %3d                                 ║\n",
           resources->process_count, MAX_PROCESSES);

    printf("╚══════════════════════════════════════════════════════════════╝\n");
    printf("\n");

    pthread_mutex_unlock(&resources->resource_lock);
}

/**
 * print_process_info - Print detailed process information
 * @process: Process to print
 *
 * Displays detailed information about a specific process.
 */
void print_process_info(Process* process) {
    if (process == NULL) {
        return;
    }

    const char* state_names[] = {"NEW", "READY", "RUNNING", "WAITING", "TERMINATED"};
    const char* priority_names[] = {"HIGH", "MEDIUM", "LOW"};

    printf("\n");
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║                     PROCESS INFORMATION                      ║\n");
    printf("╠══════════════════════════════════════════════════════════════╣\n");
    printf("║ PID:           %5d                                          ║\n", process->pid);
    printf("║ Name:          %-40s     ║\n", process->name);
    printf("║ State:         %-10s                                     ║\n",
           state_names[process->state]);
    printf("║ Priority:      %-10s                                     ║\n",
           priority_names[process->priority]);
    printf("║                                                              ║\n");
    printf("║ Resources Required:                                          ║\n");
    printf("║   RAM:         %5d MB                                       ║\n",
           process->memory_required);
    printf("║   HDD:         %5d MB                                       ║\n",
           process->hdd_required);
    printf("║   CPU Cores:   %5d                                          ║\n",
           process->cpu_cores_needed);
    printf("╚══════════════════════════════════════════════════════════════╝\n");
    printf("\n");
}

/**
 * print_banner - Print application banner
 * @app_name: Name of the application
 *
 * Displays a banner for an application.
 */
void print_banner(const char* app_name) {
    printf("\n");
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║  %-58s  ║\n", app_name);
    printf("╚══════════════════════════════════════════════════════════════╝\n");
    printf("\n");
}

/**
 * get_current_time_string - Get current time as string
 * @buffer: Buffer to store time string
 * @size: Size of buffer
 *
 * Formats current time as a readable string.
 */
void get_current_time_string(char* buffer, size_t size) {
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", tm_info);
}

/**
 * print_menu - Print menu options
 * @options: Array of menu option strings
 * @count: Number of options
 *
 * Displays a numbered menu.
 */
void print_menu(const char* options[], int count) {
    printf("\n╔══════════════════════════════════════════════════════════════╗\n");
    printf("║                          MENU                                ║\n");
    printf("╠══════════════════════════════════════════════════════════════╣\n");

    for (int i = 0; i < count; i++) {
        printf("║ %2d. %-55s║\n", i + 1, options[i]);
    }

    printf("╚══════════════════════════════════════════════════════════════╝\n");
    printf("\n");
}

/**
 * read_integer - Read integer from user
 * @prompt: Prompt to display
 *
 * Reads an integer from standard input.
 *
 * Returns: Integer entered by user
 */
int read_integer(const char* prompt) {
    int value;
    printf("%s", prompt);
    scanf("%d", &value);
    getchar();  // Consume newline
    return value;
}

/**
 * read_string - Read string from user
 * @prompt: Prompt to display
 * @buffer: Buffer to store string
 * @size: Size of buffer
 *
 * Reads a string from standard input.
 */
void read_string(const char* prompt, char* buffer, size_t size) {
    printf("%s", prompt);
    fgets(buffer, size, stdin);

    // Remove trailing newline
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
    }
}
