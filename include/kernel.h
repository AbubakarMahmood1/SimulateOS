/**
 * kernel.h - Kernel function declarations
 * Contains all function prototypes for kernel components
 */

#ifndef KERNEL_H
#define KERNEL_H

#include "types.h"
#include <stdbool.h>

// ============================================================================
// CONFIG - Configuration Management
// ============================================================================

/**
 * Load system configuration from config file
 * Returns: true on success, false on failure
 */
bool load_system_config(SystemConfig* config, const char* config_file);

// ============================================================================
// BOOT - Boot Sequence
// ============================================================================

/**
 * Display boot sequence with animation
 */
void boot_sequence(SystemConfig* config);

/**
 * Initialize all system resources and data structures
 * Returns: true on success, false on failure
 */
bool initialize_system(SystemConfig* config, SystemResources* resources);

// ============================================================================
// MEMORY - Memory Management
// ============================================================================

/**
 * Initialize memory management system
 */
void init_memory_manager(SystemResources* resources, int total_ram, int total_hdd);

/**
 * Allocate memory for a process
 * Returns: true if allocation successful, false otherwise
 */
bool allocate_memory(SystemResources* resources, int ram_mb, int hdd_mb);

/**
 * Deallocate memory for a process
 */
void deallocate_memory(SystemResources* resources, int ram_mb, int hdd_mb);

/**
 * Check if enough memory is available
 * Returns: true if resources available, false otherwise
 */
bool check_memory_available(SystemResources* resources, int ram_mb, int hdd_mb);

// ============================================================================
// PROCESS - Process Management
// ============================================================================

/**
 * Initialize process management system
 */
void init_process_manager(SystemResources* resources);

/**
 * Create a new process
 * Returns: pointer to created Process, or NULL on failure
 */
Process* create_process(const char* name, PriorityLevel priority,
                       int memory_req, int hdd_req, int cores_req,
                       SystemResources* resources);

/**
 * Terminate a process and free its resources
 */
void terminate_process(Process* process, SystemResources* resources);

/**
 * Find process by PID
 * Returns: pointer to Process, or NULL if not found
 */
Process* find_process_by_pid(SystemResources* resources, int pid);

/**
 * Update process state
 */
void update_process_state(Process* process, ProcessState new_state);

/**
 * Get next available PID
 */
int get_next_pid(void);

// ============================================================================
// SCHEDULER - Process Scheduling
// ============================================================================

/**
 * Initialize the multilevel queue scheduler
 */
MultilevelScheduler* init_scheduler(int time_quantum);

/**
 * Add process to appropriate queue based on priority
 */
void enqueue_process(MultilevelScheduler* scheduler, Process* process);

/**
 * Get next process to run based on scheduling algorithm
 * Returns: pointer to Process, or NULL if no processes ready
 */
Process* get_next_process(MultilevelScheduler* scheduler);

/**
 * Round Robin scheduling algorithm
 */
Process* schedule_round_robin(ProcessQueue* queue, int time_quantum);

/**
 * Priority scheduling algorithm
 */
Process* schedule_priority(ProcessQueue* queue);

/**
 * FCFS scheduling algorithm
 */
Process* schedule_fcfs(ProcessQueue* queue);

/**
 * Context switch to next process
 */
void context_switch(Process* current, Process* next);

// ============================================================================
// IPC - Inter-Process Communication
// ============================================================================

/**
 * Initialize IPC mechanisms
 * Returns: message queue ID
 */
int init_ipc(void);

/**
 * Send message via message queue
 * Returns: true on success, false on failure
 */
bool send_message(int msg_queue_id, IPCMessage* msg);

/**
 * Receive message from message queue
 * Returns: true on success, false on failure
 */
bool receive_message(int msg_queue_id, IPCMessage* msg, long msg_type);

/**
 * Create shared memory segment
 * Returns: shared memory ID
 */
int create_shared_memory(size_t size);

/**
 * Attach to shared memory
 * Returns: pointer to shared memory
 */
void* attach_shared_memory(int shm_id);

/**
 * Detach from shared memory
 */
void detach_shared_memory(void* shm_ptr);

/**
 * Create pipe for communication
 * Returns: true on success, false on failure
 */
bool create_pipe(int pipe_fd[2]);

// ============================================================================
// SYNC - Synchronization Primitives
// ============================================================================

/**
 * Initialize mutex
 */
void init_mutex(pthread_mutex_t* mutex);

/**
 * Lock mutex
 */
void lock_mutex(pthread_mutex_t* mutex);

/**
 * Unlock mutex
 */
void unlock_mutex(pthread_mutex_t* mutex);

/**
 * Initialize semaphore
 * Returns: pointer to semaphore
 */
void* init_semaphore(int initial_value);

/**
 * Wait on semaphore (P operation)
 */
void semaphore_wait(void* sem);

/**
 * Signal semaphore (V operation)
 */
void semaphore_signal(void* sem);

/**
 * Initialize condition variable
 */
void init_condition_var(pthread_cond_t* cond);

/**
 * Wait on condition variable
 */
void condition_wait(pthread_cond_t* cond, pthread_mutex_t* mutex);

/**
 * Signal condition variable
 */
void condition_signal(pthread_cond_t* cond);

// ============================================================================
// DEADLOCK - Deadlock Avoidance (Banker's Algorithm)
// ============================================================================

/**
 * Initialize Banker's Algorithm state
 */
void init_banker(BankerState* banker, int total_ram, int total_hdd, int total_cores);

/**
 * Request resources using Banker's Algorithm
 * Returns: true if request is safe and granted, false otherwise
 */
bool request_resources(BankerState* banker, int pid, int ram, int hdd, int cores);

/**
 * Release resources
 */
void release_resources(BankerState* banker, int pid, int ram, int hdd, int cores);

/**
 * Check if system is in safe state
 * Returns: true if safe, false if unsafe
 */
bool is_safe_state(BankerState* banker, int process_count);

// ============================================================================
// QUEUE - Queue Operations
// ============================================================================

/**
 * Create a new process queue
 */
ProcessQueue* create_queue(void);

/**
 * Enqueue process to queue
 */
void queue_enqueue(ProcessQueue* queue, Process* process);

/**
 * Dequeue process from queue
 * Returns: pointer to Process, or NULL if queue empty
 */
Process* queue_dequeue(ProcessQueue* queue);

/**
 * Check if queue is empty
 */
bool queue_is_empty(ProcessQueue* queue);

/**
 * Destroy queue and free memory
 */
void destroy_queue(ProcessQueue* queue);

// ============================================================================
// APP - Application Management
// ============================================================================

/**
 * Launch an application as a separate process
 * Returns: PID of launched application, or -1 on failure
 */
int launch_application(const char* app_name, AppDescriptor* app,
                      SystemResources* resources, MultilevelScheduler* scheduler,
                      BankerState* banker);

/**
 * Get list of available applications
 */
int get_available_apps(AppDescriptor apps[], int max_apps);

/**
 * Kill application by PID
 */
bool kill_application(int pid, SystemResources* resources);

// ============================================================================
// UTILS - Utility Functions
// ============================================================================

/**
 * Print system status
 */
void print_system_status(SystemResources* resources);

/**
 * Print process information
 */
void print_process_info(Process* process);

/**
 * Clear screen
 */
void clear_screen(void);

/**
 * Sleep for milliseconds
 */
void sleep_ms(int milliseconds);

#endif // KERNEL_H
