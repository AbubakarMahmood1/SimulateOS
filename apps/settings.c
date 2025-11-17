/**
 * settings.c - Settings Panel Application
 * Interactive system configuration editor
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define CONFIG_FILE "config/system.conf"

typedef struct {
    char os_name[50];
    char os_version[20];
    int ram_size_mb;
    int hdd_size_gb;
    int cpu_cores;
    int rr_time_quantum_ms;
    int max_processes;
} Config;

/**
 * load_config - Load configuration from file
 */
int load_config(Config* cfg) {
    FILE* fp = fopen(CONFIG_FILE, "r");
    if (fp == NULL) {
        printf("Warning: Could not open config file\n");
        return 0;
    }

    // Set defaults
    strcpy(cfg->os_name, "MiniOS");
    strcpy(cfg->os_version, "2.0");
    cfg->ram_size_mb = 2048;
    cfg->hdd_size_gb = 256;
    cfg->cpu_cores = 8;
    cfg->rr_time_quantum_ms = 100;
    cfg->max_processes = 64;

    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        if (line[0] == '#' || line[0] == '\n' || line[0] == '[') {
            continue;
        }

        char key[64], value[64];
        if (sscanf(line, "%[^=]=%s", key, value) == 2) {
            if (strcmp(key, "RAM_SIZE_MB") == 0) {
                cfg->ram_size_mb = atoi(value);
            } else if (strcmp(key, "HDD_SIZE_GB") == 0) {
                cfg->hdd_size_gb = atoi(value);
            } else if (strcmp(key, "CPU_CORES") == 0) {
                cfg->cpu_cores = atoi(value);
            } else if (strcmp(key, "RR_TIME_QUANTUM_MS") == 0) {
                cfg->rr_time_quantum_ms = atoi(value);
            } else if (strcmp(key, "MAX_PROCESSES") == 0) {
                cfg->max_processes = atoi(value);
            } else if (strcmp(key, "OS_NAME") == 0) {
                strncpy(cfg->os_name, value, sizeof(cfg->os_name) - 1);
            } else if (strcmp(key, "OS_VERSION") == 0) {
                strncpy(cfg->os_version, value, sizeof(cfg->os_version) - 1);
            }
        }
    }

    fclose(fp);
    return 1;
}

/**
 * save_config - Save configuration to file
 */
int save_config(Config* cfg) {
    FILE* fp = fopen(CONFIG_FILE, "w");
    if (fp == NULL) {
        printf("Error: Could not open config file for writing\n");
        return 0;
    }

    fprintf(fp, "# MiniOS System Configuration\n\n");
    fprintf(fp, "[HARDWARE]\n");
    fprintf(fp, "RAM_SIZE_MB=%d\n", cfg->ram_size_mb);
    fprintf(fp, "HDD_SIZE_GB=%d\n", cfg->hdd_size_gb);
    fprintf(fp, "CPU_CORES=%d\n\n", cfg->cpu_cores);
    fprintf(fp, "[SCHEDULING]\n");
    fprintf(fp, "RR_TIME_QUANTUM_MS=%d\n", cfg->rr_time_quantum_ms);
    fprintf(fp, "MAX_PROCESSES=%d\n\n", cfg->max_processes);
    fprintf(fp, "[SYSTEM]\n");
    fprintf(fp, "OS_NAME=%s\n", cfg->os_name);
    fprintf(fp, "OS_VERSION=%s\n", cfg->os_version);

    fclose(fp);
    return 1;
}

/**
 * display_config - Display current configuration
 */
void display_config(Config* cfg) {
    printf("\n");
    printf("═══════════════════════════════════════════════════════════\n");
    printf("                 CURRENT CONFIGURATION                     \n");
    printf("═══════════════════════════════════════════════════════════\n");
    printf("  1. OS Name:           %s\n", cfg->os_name);
    printf("  2. OS Version:        %s\n", cfg->os_version);
    printf("  3. RAM Size:          %d MB\n", cfg->ram_size_mb);
    printf("  4. HDD Size:          %d GB\n", cfg->hdd_size_gb);
    printf("  5. CPU Cores:         %d\n", cfg->cpu_cores);
    printf("  6. Time Quantum:      %d ms\n", cfg->rr_time_quantum_ms);
    printf("  7. Max Processes:     %d\n", cfg->max_processes);
    printf("═══════════════════════════════════════════════════════════\n");
    printf("\n");
}

/**
 * main - Settings Panel entry point
 *
 * Interactive configuration editor with read/write to actual config file.
 *
 * Returns: 0 on success
 */
int main(void) {
    Config cfg;
    char input[256];
    int choice, value;

    printf("\n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("                    SYSTEM SETTINGS                         \n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("  PID: %d\n", getpid());
    printf("  Status: Running as REAL separate process\n");
    printf("  Config File: %s\n", CONFIG_FILE);
    printf("════════════════════════════════════════════════════════════\n");

    // Load current configuration
    if (load_config(&cfg)) {
        printf("\n[LOADED] Configuration loaded from %s\n", CONFIG_FILE);
    } else {
        printf("\n[DEFAULT] Using default configuration\n");
    }

    while (1) {
        display_config(&cfg);

        printf("Commands:\n");
        printf("  edit <number> - Edit a setting (e.g., edit 3)\n");
        printf("  save          - Save configuration to file\n");
        printf("  reload        - Reload from file\n");
        printf("  help          - Show help\n");
        printf("  exit          - Exit settings\n");
        printf("\nsettings> ");
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }

        input[strcspn(input, "\n")] = 0;

        if (strlen(input) == 0) {
            continue;
        }

        if (strcmp(input, "exit") == 0 || strcmp(input, "quit") == 0) {
            printf("\nExiting settings...\n");
            break;
        }

        if (strcmp(input, "save") == 0) {
            if (save_config(&cfg)) {
                printf("\n[SAVED] Configuration saved to %s\n", CONFIG_FILE);
                printf("Note: Restart MiniOS for changes to take effect.\n");
            }
            printf("\nPress Enter to continue...");
            getchar();
            continue;
        }

        if (strcmp(input, "reload") == 0) {
            if (load_config(&cfg)) {
                printf("\n[RELOADED] Configuration reloaded from file\n");
            }
            printf("\nPress Enter to continue...");
            getchar();
            continue;
        }

        if (strcmp(input, "help") == 0) {
            printf("\nSettings Help:\n");
            printf("─────────────────────────────────────────────────────────────\n");
            printf("This app allows you to modify MiniOS configuration.\n");
            printf("Changes are saved to: %s\n", CONFIG_FILE);
            printf("You must restart MiniOS for changes to take effect.\n");
            printf("─────────────────────────────────────────────────────────────\n");
            printf("\nPress Enter to continue...");
            getchar();
            continue;
        }

        if (sscanf(input, "edit %d", &choice) == 1) {
            printf("\n");

            switch (choice) {
                case 1:
                    printf("Enter new OS Name: ");
                    fgets(cfg.os_name, sizeof(cfg.os_name), stdin);
                    cfg.os_name[strcspn(cfg.os_name, "\n")] = 0;
                    printf("OS Name updated to: %s\n", cfg.os_name);
                    break;

                case 2:
                    printf("Enter new OS Version: ");
                    fgets(cfg.os_version, sizeof(cfg.os_version), stdin);
                    cfg.os_version[strcspn(cfg.os_version, "\n")] = 0;
                    printf("OS Version updated to: %s\n", cfg.os_version);
                    break;

                case 3:
                    printf("Enter new RAM Size (MB): ");
                    scanf("%d", &value);
                    getchar();
                    if (value > 0 && value <= 32768) {
                        cfg.ram_size_mb = value;
                        printf("RAM Size updated to: %d MB\n", value);
                    } else {
                        printf("Invalid value (must be 1-32768 MB)\n");
                    }
                    break;

                case 4:
                    printf("Enter new HDD Size (GB): ");
                    scanf("%d", &value);
                    getchar();
                    if (value > 0 && value <= 2048) {
                        cfg.hdd_size_gb = value;
                        printf("HDD Size updated to: %d GB\n", value);
                    } else {
                        printf("Invalid value (must be 1-2048 GB)\n");
                    }
                    break;

                case 5:
                    printf("Enter new CPU Cores: ");
                    scanf("%d", &value);
                    getchar();
                    if (value > 0 && value <= 128) {
                        cfg.cpu_cores = value;
                        printf("CPU Cores updated to: %d\n", value);
                    } else {
                        printf("Invalid value (must be 1-128)\n");
                    }
                    break;

                case 6:
                    printf("Enter new Time Quantum (ms): ");
                    scanf("%d", &value);
                    getchar();
                    if (value > 0 && value <= 1000) {
                        cfg.rr_time_quantum_ms = value;
                        printf("Time Quantum updated to: %d ms\n", value);
                    } else {
                        printf("Invalid value (must be 1-1000 ms)\n");
                    }
                    break;

                case 7:
                    printf("Enter new Max Processes: ");
                    scanf("%d", &value);
                    getchar();
                    if (value > 0 && value <= 256) {
                        cfg.max_processes = value;
                        printf("Max Processes updated to: %d\n", value);
                    } else {
                        printf("Invalid value (must be 1-256)\n");
                    }
                    break;

                default:
                    printf("Invalid choice. Use 1-7.\n");
                    break;
            }

            printf("\nPress Enter to continue...");
            getchar();
            continue;
        }

        printf("Unknown command. Type 'help' for available commands.\n");
        printf("\nPress Enter to continue...");
        getchar();
    }

    printf("Settings closed.\n");
    return 0;
}
