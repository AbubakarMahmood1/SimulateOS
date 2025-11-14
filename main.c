/**
 * main.c - MiniOS Main Entry Point
 * Operating system simulator with process management, scheduling, and applications
 *
 * This is the main entry point for MiniOS, a comprehensive OS simulation that
 * implements process management, memory allocation, multilevel scheduling,
 * IPC mechanisms, and deadlock avoidance using Banker's Algorithm.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "include/kernel.h"
#include "include/types.h"

// Global system state
static SystemConfig config;
static SystemResources resources;
static MultilevelScheduler* scheduler;
static BankerState banker;
static int msg_queue_id;
static bool system_running = true;

// Function prototypes for external app framework functions
extern void print_available_apps(void);
extern int launch_app_by_id(int app_id, SystemResources* resources,
                            MultilevelScheduler* scheduler, BankerState* banker);
extern int get_available_apps(AppDescriptor apps[], int max_apps);

/**
 * signal_handler - Handle interrupt signals
 * @signum: Signal number
 *
 * Handles SIGINT (Ctrl+C) to allow graceful shutdown.
 */
void signal_handler(int signum) {
    if (signum == SIGINT) {
        printf("\n\n[System] Received shutdown signal...\n");
        system_running = false;
    }
}

/**
 * display_main_menu - Display the main OS menu
 *
 * Shows the main menu with options for launching apps,
 * viewing system status, and managing processes.
 */
void display_main_menu(void) {
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                              MINIOS MAIN MENU                              ║\n");
    printf("╠════════════════════════════════════════════════════════════════════════════╣\n");
    printf("║  1. Launch Application                                                     ║\n");
    printf("║  2. View System Status                                                     ║\n");
    printf("║  3. View Running Processes                                                 ║\n");
    printf("║  4. Kill Process                                                           ║\n");
    printf("║  5. View Scheduler Queues                                                  ║\n");
    printf("║  6. System Information                                                     ║\n");
    printf("║  7. Shutdown System                                                        ║\n");
    printf("╚════════════════════════════════════════════════════════════════════════════╝\n");
    printf("\n");
}

/**
 * view_running_processes - Display all running processes
 *
 * Lists all currently active processes with their details.
 */
void view_running_processes(void) {
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                           RUNNING PROCESSES                                ║\n");
    printf("╠════════════════════════════════════════════════════════════════════════════╣\n");

    pthread_mutex_lock(&resources.resource_lock);

    if (resources.process_count == 0) {
        printf("║  No processes running                                                      ║\n");
    } else {
        printf("║  PID   │ Name                  │ State   │ Priority │ RAM    │ HDD      ║\n");
        printf("║  ─────────────────────────────────────────────────────────────────────── ║\n");

        for (int i = 0; i < MAX_PROCESSES; i++) {
            if (resources.running_processes[i] != NULL) {
                Process* p = resources.running_processes[i];
                const char* state_names[] = {"NEW", "READY", "RUN", "WAIT", "TERM"};
                const char* priority_names[] = {"HIGH", "MED", "LOW"};

                printf("║  %-5d│ %-21s│ %-7s│ %-8s│ %-6dMB│ %-8dMB║\n",
                       p->pid, p->name, state_names[p->state],
                       priority_names[p->priority], p->memory_required,
                       p->hdd_required);
            }
        }
    }

    pthread_mutex_unlock(&resources.resource_lock);

    printf("╚════════════════════════════════════════════════════════════════════════════╝\n");
    printf("\n");
}

/**
 * view_scheduler_queues - Display scheduler queue status
 *
 * Shows the current state of all three priority queues.
 */
void view_scheduler_queues(void) {
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                          SCHEDULER QUEUES                                  ║\n");
    printf("╠════════════════════════════════════════════════════════════════════════════╣\n");

    printf("║  Queue Level       │ Algorithm          │ Size                            ║\n");
    printf("║  ─────────────────────────────────────────────────────────────────────────║\n");
    printf("║  High Priority     │ Round Robin        │ %d processes                    ║\n",
           scheduler->high_priority_queue->size);
    printf("║  Medium Priority   │ Priority Sched.    │ %d processes                    ║\n",
           scheduler->medium_priority_queue->size);
    printf("║  Low Priority      │ FCFS               │ %d processes                    ║\n",
           scheduler->low_priority_queue->size);

    printf("╚════════════════════════════════════════════════════════════════════════════╝\n");
    printf("\n");
}

/**
 * launch_app_menu - Display app launch menu and handle selection
 *
 * Shows available applications and launches the selected one.
 */
void launch_app_menu(void) {
    print_available_apps();

    printf("Enter application ID (1-15) or 0 to cancel: ");
    int app_id;
    scanf("%d", &app_id);
    getchar();  // Consume newline

    if (app_id == 0) {
        return;
    }

    if (app_id < 1 || app_id > 15) {
        printf("\n[Error] Invalid application ID\n");
        return;
    }

    int pid = launch_app_by_id(app_id, &resources, scheduler, &banker);

    if (pid > 0) {
        printf("\n[Success] Application launched with PID %d\n", pid);
        printf("Note: In a full implementation, this would fork and exec the actual app.\n");
    } else {
        printf("\n[Error] Failed to launch application\n");
    }

    printf("\nPress Enter to continue...");
    getchar();
}

/**
 * kill_process_menu - Display kill process menu
 *
 * Allows user to terminate a running process by PID.
 */
void kill_process_menu(void) {
    view_running_processes();

    printf("Enter PID to kill (0 to cancel): ");
    int pid;
    scanf("%d", &pid);
    getchar();  // Consume newline

    if (pid == 0) {
        return;
    }

    Process* process = find_process_by_pid(&resources, pid);

    if (process == NULL) {
        printf("\n[Error] Process with PID %d not found\n", pid);
    } else {
        // Free resources
        deallocate_memory(&resources, process->memory_required,
                         process->hdd_required);
        release_resources(&banker, pid, process->memory_required,
                         process->hdd_required, process->cpu_cores_needed);
        terminate_process(process, &resources);

        printf("\n[Success] Process %d terminated\n", pid);
    }

    printf("\nPress Enter to continue...");
    getchar();
}

/**
 * main - MiniOS entry point
 *
 * Initializes the operating system, boots up, and runs the main loop.
 *
 * Returns: 0 on successful shutdown, 1 on error
 */
int main(void) {
    // Set up signal handler for graceful shutdown
    signal(SIGINT, signal_handler);

    // Load system configuration
    if (!load_system_config(&config, "config/system.conf")) {
        printf("Error: Failed to load configuration\n");
        return 1;
    }

    // Display boot sequence
    boot_sequence(&config);

    // Initialize system components
    if (!initialize_system(&config, &resources)) {
        printf("Error: System initialization failed\n");
        return 1;
    }

    // Initialize scheduler
    scheduler = init_scheduler(config.rr_time_quantum_ms);
    if (scheduler == NULL) {
        printf("Error: Scheduler initialization failed\n");
        return 1;
    }

    // Initialize Banker's Algorithm
    init_banker(&banker, config.ram_size_mb, config.hdd_size_gb * 1024,
                config.cpu_cores);

    // Initialize IPC
    msg_queue_id = init_ipc();
    if (msg_queue_id == -1) {
        printf("Warning: IPC initialization failed, continuing without IPC\n");
    }

    printf("\n[System] All components initialized successfully!\n");
    sleep(1);

    // Main OS loop
    while (system_running) {
        clear_screen();
        display_main_menu();

        printf("Select option: ");
        int choice;
        scanf("%d", &choice);
        getchar();  // Consume newline

        switch (choice) {
            case 1:
                launch_app_menu();
                break;

            case 2:
                print_system_status(&resources);
                printf("\nPress Enter to continue...");
                getchar();
                break;

            case 3:
                view_running_processes();
                printf("Press Enter to continue...");
                getchar();
                break;

            case 4:
                kill_process_menu();
                break;

            case 5:
                view_scheduler_queues();
                printf("Press Enter to continue...");
                getchar();
                break;

            case 6:
                printf("\n");
                printf("System: %s v%s\n", config.os_name, config.os_version);
                printf("RAM: %d MB\n", config.ram_size_mb);
                printf("HDD: %d GB\n", config.hdd_size_gb);
                printf("CPU Cores: %d\n", config.cpu_cores);
                printf("Time Quantum: %d ms\n", config.rr_time_quantum_ms);
                printf("\nPress Enter to continue...");
                getchar();
                break;

            case 7:
                printf("\n[System] Shutting down...\n");
                system_running = false;
                break;

            default:
                printf("\n[Error] Invalid option\n");
                sleep(1);
                break;
        }
    }

    // Cleanup
    printf("\n[System] Cleaning up resources...\n");

    // Terminate all running processes
    pthread_mutex_lock(&resources.resource_lock);
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (resources.running_processes[i] != NULL) {
            printf("[System] Terminating process: %s\n",
                   resources.running_processes[i]->name);
            Process* p = resources.running_processes[i];
            resources.running_processes[i] = NULL;
            free(p);
        }
    }
    pthread_mutex_unlock(&resources.resource_lock);

    // Destroy scheduler queues
    destroy_queue(scheduler->high_priority_queue);
    destroy_queue(scheduler->medium_priority_queue);
    destroy_queue(scheduler->low_priority_queue);
    pthread_mutex_destroy(&scheduler->scheduler_lock);
    free(scheduler);

    // Destroy resource lock
    pthread_mutex_destroy(&resources.resource_lock);

    printf("\n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("           MiniOS has been shut down successfully           \n");
    printf("                  Thank you for using MiniOS!               \n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("\n");

    return 0;
}
