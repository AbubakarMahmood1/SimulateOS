/**
 * deadlock.c - Deadlock avoidance using Banker's Algorithm
 * Implements resource allocation safety checking
 */

#include "../include/kernel.h"
#include "../include/types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/**
 * init_banker - Initialize Banker's Algorithm state
 * @banker: Pointer to BankerState structure
 * @total_ram: Total available RAM in MB
 * @total_hdd: Total available HDD in MB
 * @total_cores: Total available CPU cores
 *
 * Initializes the Banker's Algorithm data structures with
 * total system resources. Sets up allocation matrices.
 */
void init_banker(BankerState* banker, int total_ram, int total_hdd, int total_cores) {
    if (banker == NULL) {
        return;
    }

    // Initialize maximum resources
    banker->max_resources[0] = total_ram;
    banker->max_resources[1] = total_hdd;
    banker->max_resources[2] = total_cores;

    // Initially all resources are available
    banker->available[0] = total_ram;
    banker->available[1] = total_hdd;
    banker->available[2] = total_cores;

    // Initialize all matrices to 0
    memset(banker->allocation, 0, sizeof(banker->allocation));
    memset(banker->max_need, 0, sizeof(banker->max_need));
    memset(banker->need, 0, sizeof(banker->need));

    printf("[Banker] Initialized with resources: RAM=%dMB, HDD=%dMB, Cores=%d\n",
           total_ram, total_hdd, total_cores);
}

/**
 * is_safe_state - Check if system is in safe state
 * @banker: Pointer to BankerState
 * @process_count: Number of active processes
 *
 * Implements the safety algorithm to check if a safe sequence exists.
 * A safe state means that there exists an execution order where all
 * processes can complete without deadlock.
 *
 * Returns: true if system is in safe state, false otherwise
 */
bool is_safe_state(BankerState* banker, int process_count) {
    if (banker == NULL || process_count == 0) {
        return true;
    }

    // Work arrays for simulation
    int work[3];
    bool finish[MAX_PROCESSES];
    int safe_sequence[MAX_PROCESSES];
    int count = 0;

    // Initialize work to available resources
    work[0] = banker->available[0];
    work[1] = banker->available[1];
    work[2] = banker->available[2];

    // Initialize finish array
    for (int i = 0; i < MAX_PROCESSES; i++) {
        finish[i] = false;
    }

    // Try to find a safe sequence
    while (count < process_count) {
        bool found = false;

        for (int i = 0; i < MAX_PROCESSES; i++) {
            // Skip if already finished or no allocation
            if (finish[i] || banker->max_need[i][0] == 0) {
                continue;
            }

            // Check if process can finish with available resources
            bool can_finish = true;
            for (int j = 0; j < 3; j++) {
                if (banker->need[i][j] > work[j]) {
                    can_finish = false;
                    break;
                }
            }

            if (can_finish) {
                // Process can finish, add allocated resources back to work
                for (int j = 0; j < 3; j++) {
                    work[j] += banker->allocation[i][j];
                }

                safe_sequence[count++] = i;
                finish[i] = true;
                found = true;
            }
        }

        // If no process could be found in this iteration, unsafe state
        if (!found) {
            printf("[Banker] UNSAFE STATE detected!\n");
            return false;
        }
    }

    printf("[Banker] System is in SAFE state. Safe sequence: ");
    for (int i = 0; i < count; i++) {
        printf("P%d ", safe_sequence[i]);
    }
    printf("\n");

    return true;
}

/**
 * request_resources - Request resources using Banker's Algorithm
 * @banker: Pointer to BankerState
 * @pid: Process ID (used as index)
 * @ram: RAM requested in MB
 * @hdd: HDD requested in MB
 * @cores: CPU cores requested
 *
 * Attempts to allocate resources to a process. Checks if the allocation
 * would leave the system in a safe state before granting the request.
 *
 * Returns: true if request granted (safe), false if denied (unsafe)
 */
bool request_resources(BankerState* banker, int pid, int ram, int hdd, int cores) {
    if (banker == NULL) {
        return false;
    }

    // Use PID modulo MAX_PROCESSES as index
    int index = pid % MAX_PROCESSES;

    printf("[Banker] Process %d requesting: RAM=%dMB, HDD=%dMB, Cores=%d\n",
           pid, ram, hdd, cores);

    // Check if request exceeds maximum need
    if (banker->max_need[index][0] == 0) {
        // First request from this process, set maximum need
        banker->max_need[index][0] = ram;
        banker->max_need[index][1] = hdd;
        banker->max_need[index][2] = cores;
    }

    // Check if request exceeds available resources
    if (ram > banker->available[0] || hdd > banker->available[1] ||
        cores > banker->available[2]) {
        printf("[Banker] Request denied: Exceeds available resources\n");
        printf("         Available: RAM=%dMB, HDD=%dMB, Cores=%d\n",
               banker->available[0], banker->available[1], banker->available[2]);
        return false;
    }

    // Tentatively allocate resources
    banker->available[0] -= ram;
    banker->available[1] -= hdd;
    banker->available[2] -= cores;

    banker->allocation[index][0] += ram;
    banker->allocation[index][1] += hdd;
    banker->allocation[index][2] += cores;

    banker->need[index][0] = banker->max_need[index][0] - banker->allocation[index][0];
    banker->need[index][1] = banker->max_need[index][1] - banker->allocation[index][1];
    banker->need[index][2] = banker->max_need[index][2] - banker->allocation[index][2];

    // Count active processes
    int process_count = 0;
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (banker->max_need[i][0] > 0) {
            process_count++;
        }
    }

    // Check if system is still in safe state
    if (is_safe_state(banker, process_count)) {
        printf("[Banker] Request GRANTED - System remains in safe state\n");
        return true;
    } else {
        // Rollback allocation
        banker->available[0] += ram;
        banker->available[1] += hdd;
        banker->available[2] += cores;

        banker->allocation[index][0] -= ram;
        banker->allocation[index][1] -= hdd;
        banker->allocation[index][2] -= cores;

        banker->need[index][0] = banker->max_need[index][0] - banker->allocation[index][0];
        banker->need[index][1] = banker->max_need[index][1] - banker->allocation[index][1];
        banker->need[index][2] = banker->max_need[index][2] - banker->allocation[index][2];

        printf("[Banker] Request DENIED - Would lead to unsafe state\n");
        return false;
    }
}

/**
 * release_resources - Release allocated resources
 * @banker: Pointer to BankerState
 * @pid: Process ID
 * @ram: RAM to release in MB
 * @hdd: HDD to release in MB
 * @cores: CPU cores to release
 *
 * Returns previously allocated resources back to the available pool.
 * Called when a process terminates or explicitly releases resources.
 */
void release_resources(BankerState* banker, int pid, int ram, int hdd, int cores) {
    if (banker == NULL) {
        return;
    }

    int index = pid % MAX_PROCESSES;

    printf("[Banker] Process %d releasing: RAM=%dMB, HDD=%dMB, Cores=%d\n",
           pid, ram, hdd, cores);

    // Return resources to available pool
    banker->available[0] += ram;
    banker->available[1] += hdd;
    banker->available[2] += cores;

    // Update allocation
    banker->allocation[index][0] -= ram;
    banker->allocation[index][1] -= hdd;
    banker->allocation[index][2] -= cores;

    // Ensure non-negative
    if (banker->allocation[index][0] < 0) banker->allocation[index][0] = 0;
    if (banker->allocation[index][1] < 0) banker->allocation[index][1] = 0;
    if (banker->allocation[index][2] < 0) banker->allocation[index][2] = 0;

    // Update need
    banker->need[index][0] = banker->max_need[index][0] - banker->allocation[index][0];
    banker->need[index][1] = banker->max_need[index][1] - banker->allocation[index][1];
    banker->need[index][2] = banker->max_need[index][2] - banker->allocation[index][2];

    // If all resources released, clear max_need
    if (banker->allocation[index][0] == 0 &&
        banker->allocation[index][1] == 0 &&
        banker->allocation[index][2] == 0) {
        banker->max_need[index][0] = 0;
        banker->max_need[index][1] = 0;
        banker->max_need[index][2] = 0;
        banker->need[index][0] = 0;
        banker->need[index][1] = 0;
        banker->need[index][2] = 0;
    }

    printf("[Banker] Resources released successfully\n");
    printf("         Available: RAM=%dMB, HDD=%dMB, Cores=%d\n",
           banker->available[0], banker->available[1], banker->available[2]);
}
