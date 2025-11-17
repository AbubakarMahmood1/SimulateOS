/**
 * process_manager.c - Process Manager Application
 * List and manage system processes
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <signal.h>
#include <ctype.h>

void list_processes(void) {
    DIR *dir = opendir("/proc");
    if (!dir) {
        printf("Error: Cannot read /proc\n");
        return;
    }
    
    printf("\n═══════════════════════════════════════════════════════════════════\n");
    printf("%-8s %-20s %-10s %s\n", "PID", "Name", "State", "Memory");
    printf("───────────────────────────────────────────────────────────────────\n");
    
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (!isdigit(entry->d_name[0])) continue;
        
        char path[512], line[256];
        char name[256] = "", state[10] = "";
        int vmsize = 0;
        
        // Read process name and state
        snprintf(path, sizeof(path), "/proc/%s/status", entry->d_name);
        FILE *fp = fopen(path, "r");
        if (fp) {
            while (fgets(line, sizeof(line), fp)) {
                if (strncmp(line, "Name:", 5) == 0) {
                    sscanf(line + 6, "%s", name);
                } else if (strncmp(line, "State:", 6) == 0) {
                    sscanf(line + 7, "%s", state);
                } else if (strncmp(line, "VmSize:", 7) == 0) {
                    sscanf(line + 8, "%d", &vmsize);
                }
            }
            fclose(fp);
            printf("%-8s %-20s %-10s %d KB\n", entry->d_name, name, state, vmsize);
        }
    }
    printf("═══════════════════════════════════════════════════════════════════\n\n");
    closedir(dir);
}

int main(void) {
    char input[256];
    int pid;
    
    printf("\n════════════════════════════════════════════════════════════\n");
    printf("                   PROCESS MANAGER                          \n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("  PID: %d\n", getpid());
    printf("  Status: Running as REAL separate process\n");
    printf("════════════════════════════════════════════════════════════\n");
    
    while (1) {
        list_processes();
        
        printf("Commands: refresh, kill <pid>, exit\n");
        printf("procmgr> ");
        fflush(stdout);
        
        if (fgets(input, sizeof(input), stdin) == NULL) break;
        input[strcspn(input, "\n")] = 0;
        
        if (strcmp(input, "exit") == 0) break;
        if (strcmp(input, "refresh") == 0) continue;
        
        if (sscanf(input, "kill %d", &pid) == 1) {
            if (kill(pid, SIGTERM) == 0) {
                printf("Sent SIGTERM to process %d\n", pid);
            } else {
                printf("Error: Cannot kill process %d\n", pid);
            }
            sleep(1);
        }
    }
    
    printf("\nProcess Manager closing...\n");
    return 0;
}
