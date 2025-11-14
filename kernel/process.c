/**
 * process.c - Process management system
 * Handles process creation, termination, and state management
 */

#include "../include/kernel.h"
#include "../include/types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

static int next_pid = 1000;  // Start PIDs from 1000

/**
 * init_process_manager - Initialize process management system
 * @resources: System resources structure
 *
 * Sets up process tracking structures and initializes
 * the process table.
 */
void init_process_manager(SystemResources* resources) {
    resources->process_count = 0;
    resources->total_cores = 8;  // Default, can be configured
    resources->available_cores = 8;

    // Initialize all process slots to NULL
    for (int i = 0; i < MAX_PROCESSES; i++) {
        resources->running_processes[i] = NULL;
    }

    printf("[Process Manager] Initialized (Max processes: %d)\n", MAX_PROCESSES);
}

/**
 * get_next_pid - Get next available process ID
 *
 * Returns a unique PID for a new process.
 * Thread-safe implementation using atomic increment.
 *
 * Returns: Next available PID
 */
int get_next_pid(void) {
    return __sync_fetch_and_add(&next_pid, 1);
}

/**
 * create_process - Create a new process
 * @name: Process name
 * @priority: Process priority level
 * @memory_req: Required RAM in MB
 * @hdd_req: Required HDD in MB
 * @cores_req: Required CPU cores
 * @resources: System resources structure
 *
 * Creates a new process with the specified parameters.
 * Allocates a PCB and initializes all process attributes.
 * Does not allocate actual resources - that's done by the launcher.
 *
 * Returns: Pointer to created Process, or NULL on failure
 */
Process* create_process(const char* name, PriorityLevel priority,
                       int memory_req, int hdd_req, int cores_req,
                       SystemResources* resources) {

    pthread_mutex_lock(&resources->resource_lock);

    // Check if we've reached maximum process limit
    if (resources->process_count >= MAX_PROCESSES) {
        pthread_mutex_unlock(&resources->resource_lock);
        printf("[Process Manager] Error: Maximum process limit reached\n");
        return NULL;
    }

    // Allocate PCB
    Process* process = (Process*)malloc(sizeof(Process));
    if (process == NULL) {
        pthread_mutex_unlock(&resources->resource_lock);
        printf("[Process Manager] Error: Failed to allocate PCB\n");
        return NULL;
    }

    // Initialize process attributes
    process->pid = get_next_pid();
    strncpy(process->name, name, MAX_NAME_LEN - 1);
    process->name[MAX_NAME_LEN - 1] = '\0';
    process->state = PROCESS_NEW;
    process->priority = priority;

    process->memory_required = memory_req;
    process->hdd_required = hdd_req;
    process->cpu_cores_needed = cores_req;

    process->creation_time = time(NULL);
    process->burst_time = 0;
    process->time_quantum_remaining = 0;
    process->waiting_time = 0;

    process->system_pid = 0;  // Will be set when actually forked
    process->parent_pid = getpid();

    // Add to process table
    int slot = -1;
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (resources->running_processes[i] == NULL) {
            resources->running_processes[i] = process;
            slot = i;
            break;
        }
    }

    if (slot == -1) {
        free(process);
        pthread_mutex_unlock(&resources->resource_lock);
        printf("[Process Manager] Error: No available process slot\n");
        return NULL;
    }

    resources->process_count++;

    pthread_mutex_unlock(&resources->resource_lock);

    printf("[Process Manager] Created process: PID=%d, Name=%s, Priority=%d\n",
           process->pid, process->name, process->priority);

    return process;
}

/**
 * terminate_process - Terminate a process
 * @process: Process to terminate
 * @resources: System resources structure
 *
 * Terminates a process and frees all associated resources.
 * Updates process table and resource counters.
 */
void terminate_process(Process* process, SystemResources* resources) {
    if (process == NULL) {
        return;
    }

    pthread_mutex_lock(&resources->resource_lock);

    printf("[Process Manager] Terminating process: PID=%d, Name=%s\n",
           process->pid, process->name);

    // Update state
    process->state = PROCESS_TERMINATED;

    // Remove from process table
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (resources->running_processes[i] == process) {
            resources->running_processes[i] = NULL;
            break;
        }
    }

    resources->process_count--;

    pthread_mutex_unlock(&resources->resource_lock);

    // Free PCB
    free(process);
}

/**
 * find_process_by_pid - Find process by PID
 * @resources: System resources structure
 * @pid: Process ID to search for
 *
 * Searches the process table for a process with the given PID.
 *
 * Returns: Pointer to Process if found, NULL otherwise
 */
Process* find_process_by_pid(SystemResources* resources, int pid) {
    pthread_mutex_lock(&resources->resource_lock);

    Process* result = NULL;
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (resources->running_processes[i] != NULL &&
            resources->running_processes[i]->pid == pid) {
            result = resources->running_processes[i];
            break;
        }
    }

    pthread_mutex_unlock(&resources->resource_lock);

    return result;
}

/**
 * update_process_state - Update process state
 * @process: Process to update
 * @new_state: New state
 *
 * Updates the state of a process. Used for state transitions
 * in the process lifecycle.
 */
void update_process_state(Process* process, ProcessState new_state) {
    if (process == NULL) {
        return;
    }

    ProcessState old_state = process->state;
    process->state = new_state;

    const char* state_names[] = {"NEW", "READY", "RUNNING", "WAITING", "TERMINATED"};

    printf("[Process Manager] PID=%d state change: %s -> %s\n",
           process->pid, state_names[old_state], state_names[new_state]);
}
