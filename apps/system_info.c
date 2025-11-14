/**
 * system_info.c - System Information Application
 * Display hardware and resource information
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/utsname.h>

/**
 * main - System Info entry point
 *
 * Displays detailed system and hardware information.
 *
 * Returns: 0 on success
 */
int main(void) {
    struct utsname sys_info;
    uname(&sys_info);

    printf("\n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("                   SYSTEM INFORMATION                       \n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("  PID: %d\n", getpid());
    printf("  Status: Running as separate process\n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("\n");

    printf("Operating System:\n");
    printf("─────────────────────────────────────────────────────────────\n");
    printf("  OS Name:           MiniOS\n");
    printf("  Version:           2.0\n");
    printf("  Kernel:            %s\n", sys_info.sysname);
    printf("  Hostname:          %s\n", sys_info.nodename);
    printf("─────────────────────────────────────────────────────────────\n");
    printf("\n");

    printf("Hardware Configuration:\n");
    printf("─────────────────────────────────────────────────────────────\n");
    printf("  CPU Cores:         8\n");
    printf("  CPU Usage:         35%%\n");
    printf("  RAM Total:         2048 MB\n");
    printf("  RAM Used:          1024 MB (50%%)\n");
    printf("  RAM Available:     1024 MB\n");
    printf("  HDD Total:         256 GB\n");
    printf("  HDD Used:          128 GB (50%%)\n");
    printf("  HDD Available:     128 GB\n");
    printf("─────────────────────────────────────────────────────────────\n");
    printf("\n");

    printf("Scheduler Configuration:\n");
    printf("─────────────────────────────────────────────────────────────\n");
    printf("  Type:              Multilevel Queue\n");
    printf("  Levels:            3 (High, Medium, Low)\n");
    printf("  Time Quantum:      100 ms\n");
    printf("  Active Processes:  12\n");
    printf("  Max Processes:     64\n");
    printf("─────────────────────────────────────────────────────────────\n");
    printf("\n");

    printf("Press Enter to close...");
    getchar();

    return 0;
}
