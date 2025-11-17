/**
 * system_monitor.c - System Monitor Application
 * Real-time system resource monitoring
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

void read_cpu_usage(void) {
    FILE *fp = fopen("/proc/stat", "r");
    if (!fp) return;
    
    unsigned long user, nice, system, idle;
    fscanf(fp, "cpu %lu %lu %lu %lu", &user, &nice, &system, &idle);
    fclose(fp);
    
    unsigned long total = user + nice + system + idle;
    unsigned long used = user + nice + system;
    int cpu_percent = (used * 100) / total;
    
    printf("  CPU Usage:         %d%%\n", cpu_percent);
}

void read_memory_usage(void) {
    FILE *fp = fopen("/proc/meminfo", "r");
    if (!fp) return;
    
    char line[256];
    long total = 0, free = 0, available = 0;
    
    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "MemTotal:", 9) == 0) {
            sscanf(line + 10, "%ld", &total);
        } else if (strncmp(line, "MemFree:", 8) == 0) {
            sscanf(line + 9, "%ld", &free);
        } else if (strncmp(line, "MemAvailable:", 13) == 0) {
            sscanf(line + 14, "%ld", &available);
            break;
        }
    }
    fclose(fp);
    
    long used = total - available;
    int percent = (used * 100) / total;
    
    printf("  RAM Usage:         %ld/%ld MB (%d%%)\n", used/1024, total/1024, percent);
}

void read_disk_usage(void) {
    FILE *fp = popen("df -h / | tail -1", "r");
    if (!fp) return;
    
    char line[256];
    char filesystem[100], size[20], used[20], avail[20], percent[10], mount[100];
    
    if (fgets(line, sizeof(line), fp)) {
        sscanf(line, "%s %s %s %s %s %s", filesystem, size, used, avail, percent, mount);
        printf("  Disk Usage:        %s / %s (%s)\n", used, size, percent);
    }
    pclose(fp);
}

void display_monitor(void) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char time_str[64];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_info);
    
    printf("\033[2J\033[H");  // Clear screen
    printf("════════════════════════════════════════════════════════════\n");
    printf("                   SYSTEM MONITOR                           \n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("  PID: %d\n", getpid());
    printf("  Time: %s\n", time_str);
    printf("════════════════════════════════════════════════════════════\n\n");
    
    printf("Resource Usage:\n");
    printf("─────────────────────────────────────────────────────────────\n");
    read_cpu_usage();
    read_memory_usage();
    read_disk_usage();
    
    FILE *fp = fopen("/proc/loadavg", "r");
    if (fp) {
        float load1, load5, load15;
        fscanf(fp, "%f %f %f", &load1, &load5, &load15);
        printf("  Load Average:      %.2f, %.2f, %.2f\n", load1, load5, load15);
        fclose(fp);
    }
    
    printf("─────────────────────────────────────────────────────────────\n");
    printf("\nPress Ctrl+C to exit | Auto-refresh: 2 seconds\n");
}

int main(void) {
    printf("\n════════════════════════════════════════════════════════════\n");
    printf("  System Monitor starting... (Updates every 2 seconds)\n");
    printf("  Press Ctrl+C to exit\n");
    printf("════════════════════════════════════════════════════════════\n");
    sleep(2);
    
    while (1) {
        display_monitor();
        sleep(2);
    }
    
    return 0;
}
