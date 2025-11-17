/**
 * system_info.c - System Information Application
 * Displays REAL hardware and resource information from /proc
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <sys/sysinfo.h>

/**
 * read_cpu_info - Read CPU information from /proc/cpuinfo
 */
void read_cpu_info(void) {
    FILE* fp = fopen("/proc/cpuinfo", "r");
    if (fp == NULL) {
        printf("  Unable to read CPU info\n");
        return;
    }

    char line[256];
    char model[256] = "Unknown";
    int cores = 0;
    float mhz = 0.0;

    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "model name", 10) == 0) {
            char* colon = strchr(line, ':');
            if (colon && cores == 0) {  // Only get first model name
                sscanf(colon + 2, "%[^\n]", model);
            }
        } else if (strncmp(line, "processor", 9) == 0) {
            cores++;
        } else if (strncmp(line, "cpu MHz", 7) == 0 && mhz == 0.0) {
            char* colon = strchr(line, ':');
            if (colon) {
                sscanf(colon + 2, "%f", &mhz);
            }
        }
    }

    fclose(fp);

    printf("  CPU Model:         %s\n", model);
    printf("  CPU Cores:         %d\n", cores);
    if (mhz > 0) {
        printf("  CPU Speed:         %.2f MHz\n", mhz);
    }
}

/**
 * read_memory_info - Read memory information from /proc/meminfo
 */
void read_memory_info(void) {
    FILE* fp = fopen("/proc/meminfo", "r");
    if (fp == NULL) {
        printf("  Unable to read memory info\n");
        return;
    }

    char line[256];
    long total_kb = 0, free_kb = 0, available_kb = 0;

    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "MemTotal:", 9) == 0) {
            sscanf(line + 10, "%ld", &total_kb);
        } else if (strncmp(line, "MemFree:", 8) == 0) {
            sscanf(line + 9, "%ld", &free_kb);
        } else if (strncmp(line, "MemAvailable:", 13) == 0) {
            sscanf(line + 14, "%ld", &available_kb);
        }
    }

    fclose(fp);

    long total_mb = total_kb / 1024;
    long used_mb = (total_kb - available_kb) / 1024;
    long available_mb = available_kb / 1024;
    int used_percent = (total_kb > 0) ? (int)((total_kb - available_kb) * 100 / total_kb) : 0;

    printf("  RAM Total:         %ld MB\n", total_mb);
    printf("  RAM Used:          %ld MB (%d%%)\n", used_mb, used_percent);
    printf("  RAM Available:     %ld MB\n", available_mb);
}

/**
 * read_uptime - Read system uptime
 */
void read_uptime(void) {
    struct sysinfo si;
    if (sysinfo(&si) == 0) {
        long days = si.uptime / 86400;
        long hours = (si.uptime % 86400) / 3600;
        long minutes = (si.uptime % 3600) / 60;

        printf("  Uptime:            %ld days, %ld hours, %ld minutes\n",
               days, hours, minutes);
    }
}

/**
 * read_load_average - Read system load average
 */
void read_load_average(void) {
    FILE* fp = fopen("/proc/loadavg", "r");
    if (fp == NULL) {
        return;
    }

    float load1, load5, load15;
    if (fscanf(fp, "%f %f %f", &load1, &load5, &load15) == 3) {
        printf("  Load Average:      %.2f, %.2f, %.2f (1, 5, 15 min)\n",
               load1, load5, load15);
    }

    fclose(fp);
}

/**
 * count_processes - Count running processes
 */
int count_processes(void) {
    FILE* fp = fopen("/proc/stat", "r");
    if (fp == NULL) {
        return 0;
    }

    char line[256];
    int processes = 0;

    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "processes", 9) == 0) {
            sscanf(line + 10, "%d", &processes);
            break;
        }
    }

    fclose(fp);
    return processes;
}

/**
 * main - System Info entry point
 *
 * Displays REAL system and hardware information by reading
 * from /proc filesystem.
 *
 * Returns: 0 on success
 */
int main(void) {
    struct utsname uname_info;
    uname(&uname_info);

    printf("\n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("                   SYSTEM INFORMATION                       \n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("  PID: %d\n", getpid());
    printf("  Status: Running as REAL separate process\n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("\n");

    printf("Operating System:\n");
    printf("─────────────────────────────────────────────────────────────\n");
    printf("  OS Name:           MiniOS (running on %s)\n", uname_info.sysname);
    printf("  Version:           2.0\n");
    printf("  Kernel:            %s %s\n", uname_info.sysname, uname_info.release);
    printf("  Architecture:      %s\n", uname_info.machine);
    printf("  Hostname:          %s\n", uname_info.nodename);
    read_uptime();
    read_load_average();
    printf("─────────────────────────────────────────────────────────────\n");
    printf("\n");

    printf("Hardware Configuration:\n");
    printf("─────────────────────────────────────────────────────────────\n");
    read_cpu_info();
    printf("\n");
    read_memory_info();
    printf("─────────────────────────────────────────────────────────────\n");
    printf("\n");

    printf("Process Information:\n");
    printf("─────────────────────────────────────────────────────────────\n");
    int total_procs = count_processes();
    if (total_procs > 0) {
        printf("  Total Processes:   %d (since boot)\n", total_procs);
    }
    printf("  Current PID:       %d\n", getpid());
    printf("  Parent PID:        %d\n", getppid());
    printf("─────────────────────────────────────────────────────────────\n");
    printf("\n");

    printf("MiniOS Simulation Parameters:\n");
    printf("─────────────────────────────────────────────────────────────\n");
    printf("  Simulated RAM:     2048 MB\n");
    printf("  Simulated HDD:     256 GB\n");
    printf("  Simulated Cores:   8\n");
    printf("  Scheduler:         Multilevel Queue (RR/Priority/FCFS)\n");
    printf("  Max Processes:     64\n");
    printf("  Time Quantum:      100 ms\n");
    printf("─────────────────────────────────────────────────────────────\n");
    printf("\n");

    printf("Press Enter to close...");
    getchar();

    return 0;
}
