/**
 * boot.c - Boot sequence and system initialization
 * Handles the OS boot process and system setup
 */

#include "../include/kernel.h"
#include "../include/types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/**
 * load_system_config - Load system configuration from file
 * @config: Pointer to SystemConfig structure
 * @config_file: Path to configuration file
 *
 * Reads configuration parameters from the config file and populates
 * the SystemConfig structure. Uses default values if file cannot be read.
 *
 * Returns: true on success, false on failure
 */
bool load_system_config(SystemConfig* config, const char* config_file) {
    FILE* file = fopen(config_file, "r");

    // Set defaults
    strcpy(config->os_name, "MiniOS");
    strcpy(config->os_version, "2.0");
    config->ram_size_mb = 2048;
    config->hdd_size_gb = 256;
    config->cpu_cores = 8;
    config->rr_time_quantum_ms = 100;
    config->max_processes = MAX_PROCESSES;

    if (file == NULL) {
        printf("Warning: Could not open config file, using defaults\n");
        return true;  // Use defaults
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        // Skip comments and empty lines
        if (line[0] == '#' || line[0] == '\n' || line[0] == '[') {
            continue;
        }

        // Parse key=value pairs
        char key[64], value[64];
        if (sscanf(line, "%[^=]=%s", key, value) == 2) {
            if (strcmp(key, "RAM_SIZE_MB") == 0) {
                config->ram_size_mb = atoi(value);
            } else if (strcmp(key, "HDD_SIZE_GB") == 0) {
                config->hdd_size_gb = atoi(value);
            } else if (strcmp(key, "CPU_CORES") == 0) {
                config->cpu_cores = atoi(value);
            } else if (strcmp(key, "RR_TIME_QUANTUM_MS") == 0) {
                config->rr_time_quantum_ms = atoi(value);
            } else if (strcmp(key, "MAX_PROCESSES") == 0) {
                config->max_processes = atoi(value);
            } else if (strcmp(key, "OS_NAME") == 0) {
                strncpy(config->os_name, value, sizeof(config->os_name) - 1);
            } else if (strcmp(key, "OS_VERSION") == 0) {
                strncpy(config->os_version, value, sizeof(config->os_version) - 1);
            }
        }
    }

    fclose(file);
    return true;
}

/**
 * boot_sequence - Display animated boot sequence
 * @config: System configuration
 *
 * Displays a boot animation with loading messages to simulate
 * the operating system boot process.
 */
void boot_sequence(SystemConfig* config) {
    clear_screen();

    printf("\n");
    printf("================================================================================\n");
    printf("                          %s v%s Booting...                          \n",
           config->os_name, config->os_version);
    printf("================================================================================\n");
    printf("\n");

    sleep(1);

    printf("[*] Initializing hardware...\n");
    printf("    - RAM: %d MB\n", config->ram_size_mb);
    printf("    - HDD: %d GB\n", config->hdd_size_gb);
    printf("    - CPU Cores: %d\n", config->cpu_cores);
    sleep(1);

    printf("\n[*] Loading kernel modules...\n");
    printf("    - Memory Manager\n");
    sleep_ms(300);
    printf("    - Process Manager\n");
    sleep_ms(300);
    printf("    - Scheduler\n");
    sleep_ms(300);
    printf("    - IPC System\n");
    sleep_ms(300);
    printf("    - Deadlock Avoidance\n");
    sleep(1);

    printf("\n[*] Starting system services...\n");
    printf("    - Resource Management\n");
    sleep_ms(300);
    printf("    - Application Framework\n");
    sleep_ms(300);
    printf("    - User Interface\n");
    sleep(1);

    printf("\n[*] System initialization complete!\n");
    sleep(1);

    printf("\n");
    printf("================================================================================\n");
    printf("                    Welcome to %s v%s!                    \n",
           config->os_name, config->os_version);
    printf("================================================================================\n");
    printf("\n");
    sleep(2);
}

/**
 * initialize_system - Initialize all system components
 * @config: System configuration
 * @resources: System resources structure
 *
 * Initializes memory, process management, and other core systems.
 * Allocates and sets up all necessary data structures.
 *
 * Returns: true on success, false on failure
 */
bool initialize_system(SystemConfig* config, SystemResources* resources) {
    // Initialize memory manager
    init_memory_manager(resources, config->ram_size_mb,
                       config->hdd_size_gb * 1024);  // Convert GB to MB

    // Initialize process manager
    init_process_manager(resources);

    // Initialize resource lock
    pthread_mutex_init(&resources->resource_lock, NULL);

    return true;
}
