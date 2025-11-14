/**
 * memory.c - Memory management system
 * Handles RAM and HDD allocation/deallocation
 */

#include "../include/kernel.h"
#include "../include/types.h"
#include <stdio.h>
#include <pthread.h>

/**
 * init_memory_manager - Initialize the memory management system
 * @resources: System resources structure
 * @total_ram: Total RAM in MB
 * @total_hdd: Total HDD in MB
 *
 * Sets up the memory manager with specified total resources.
 * Initializes both RAM and HDD tracking.
 */
void init_memory_manager(SystemResources* resources, int total_ram, int total_hdd) {
    resources->total_ram = total_ram;
    resources->total_hdd = total_hdd;
    resources->available_ram = total_ram;
    resources->available_hdd = total_hdd;

    printf("[Memory Manager] Initialized with %d MB RAM, %d MB HDD\n",
           total_ram, total_hdd);
}

/**
 * check_memory_available - Check if requested memory is available
 * @resources: System resources structure
 * @ram_mb: RAM requested in MB
 * @hdd_mb: HDD requested in MB
 *
 * Verifies if the system has enough free RAM and HDD to satisfy
 * the requested allocation.
 *
 * Returns: true if resources available, false otherwise
 */
bool check_memory_available(SystemResources* resources, int ram_mb, int hdd_mb) {
    pthread_mutex_lock(&resources->resource_lock);

    bool available = (resources->available_ram >= ram_mb) &&
                     (resources->available_hdd >= hdd_mb);

    pthread_mutex_unlock(&resources->resource_lock);

    return available;
}

/**
 * allocate_memory - Allocate memory resources
 * @resources: System resources structure
 * @ram_mb: RAM to allocate in MB
 * @hdd_mb: HDD to allocate in MB
 *
 * Allocates the specified amount of RAM and HDD if available.
 * Uses mutex to ensure thread-safe allocation.
 *
 * Returns: true if allocation successful, false otherwise
 */
bool allocate_memory(SystemResources* resources, int ram_mb, int hdd_mb) {
    pthread_mutex_lock(&resources->resource_lock);

    // Check if resources are available
    if (resources->available_ram < ram_mb || resources->available_hdd < hdd_mb) {
        pthread_mutex_unlock(&resources->resource_lock);
        printf("[Memory Manager] Allocation failed: Insufficient resources\n");
        printf("                 Requested: %d MB RAM, %d MB HDD\n", ram_mb, hdd_mb);
        printf("                 Available: %d MB RAM, %d MB HDD\n",
               resources->available_ram, resources->available_hdd);
        return false;
    }

    // Allocate resources
    resources->available_ram -= ram_mb;
    resources->available_hdd -= hdd_mb;

    pthread_mutex_unlock(&resources->resource_lock);

    printf("[Memory Manager] Allocated: %d MB RAM, %d MB HDD\n", ram_mb, hdd_mb);
    printf("                 Remaining: %d MB RAM, %d MB HDD\n",
           resources->available_ram, resources->available_hdd);

    return true;
}

/**
 * deallocate_memory - Free memory resources
 * @resources: System resources structure
 * @ram_mb: RAM to free in MB
 * @hdd_mb: HDD to free in MB
 *
 * Returns previously allocated RAM and HDD back to the available pool.
 * Uses mutex to ensure thread-safe deallocation.
 */
void deallocate_memory(SystemResources* resources, int ram_mb, int hdd_mb) {
    pthread_mutex_lock(&resources->resource_lock);

    resources->available_ram += ram_mb;
    resources->available_hdd += hdd_mb;

    // Ensure we don't exceed total resources (safety check)
    if (resources->available_ram > resources->total_ram) {
        resources->available_ram = resources->total_ram;
    }
    if (resources->available_hdd > resources->total_hdd) {
        resources->available_hdd = resources->total_hdd;
    }

    pthread_mutex_unlock(&resources->resource_lock);

    printf("[Memory Manager] Deallocated: %d MB RAM, %d MB HDD\n", ram_mb, hdd_mb);
    printf("                 Available: %d MB RAM, %d MB HDD\n",
           resources->available_ram, resources->available_hdd);
}
