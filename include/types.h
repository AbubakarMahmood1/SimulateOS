/**
 * types.h - Core data structures for MiniOS
 * Defines all essential types used throughout the operating system
 */

#ifndef TYPES_H
#define TYPES_H

#include <pthread.h>
#include <sys/types.h>
#include <time.h>

#define MAX_PROCESSES 64
#define MAX_NAME_LEN 50
#define MAX_PATH_LEN 256
#define MAX_THREADS 4

/**
 * Process States
 */
typedef enum {
    PROCESS_NEW,        // Just created
    PROCESS_READY,      // Ready to run
    PROCESS_RUNNING,    // Currently executing
    PROCESS_WAITING,    // Waiting for I/O or resource
    PROCESS_TERMINATED  // Finished execution
} ProcessState;

/**
 * Process Priority Levels (for multilevel queue)
 */
typedef enum {
    PRIORITY_HIGH = 0,      // Interactive tasks (Round Robin)
    PRIORITY_MEDIUM = 1,    // Background tasks (Priority Scheduling)
    PRIORITY_LOW = 2        // System tasks (FCFS)
} PriorityLevel;

/**
 * Application Categories
 */
typedef enum {
    APP_SYSTEM,
    APP_PRODUCTIVITY,
    APP_UTILITY,
    APP_ENTERTAINMENT
} AppCategory;

/**
 * Process Control Block (PCB)
 */
typedef struct {
    int pid;                        // Process ID
    char name[MAX_NAME_LEN];        // Process name
    ProcessState state;             // Current state
    PriorityLevel priority;         // Priority level

    // Resource requirements
    int memory_required;            // RAM in MB
    int hdd_required;               // HDD in MB
    int cpu_cores_needed;           // Number of cores

    // Timing information
    time_t creation_time;           // When process was created
    int burst_time;                 // CPU burst time in ms
    int time_quantum_remaining;     // For Round Robin
    int waiting_time;               // Time spent waiting

    // Process management
    pid_t system_pid;               // Actual system process ID
    int parent_pid;                 // Parent process ID
} Process;

/**
 * System Resources Structure
 */
typedef struct {
    // Total resources
    int total_ram;                  // Total RAM in MB
    int total_hdd;                  // Total HDD in MB
    int total_cores;                // Total CPU cores

    // Available resources
    int available_ram;              // Available RAM in MB
    int available_hdd;              // Available HDD in MB
    int available_cores;            // Available CPU cores

    // Process tracking
    Process* running_processes[MAX_PROCESSES];
    int process_count;              // Current number of processes

    // Synchronization
    pthread_mutex_t resource_lock;  // Mutex for resource access
} SystemResources;

/**
 * Scheduler Queue Node
 */
typedef struct QueueNode {
    Process* process;
    struct QueueNode* next;
} QueueNode;

/**
 * Scheduler Queue
 */
typedef struct {
    QueueNode* front;
    QueueNode* rear;
    int size;
    pthread_mutex_t queue_lock;
} ProcessQueue;

/**
 * Multilevel Queue Scheduler
 */
typedef struct {
    ProcessQueue* high_priority_queue;      // Round Robin
    ProcessQueue* medium_priority_queue;    // Priority Scheduling
    ProcessQueue* low_priority_queue;       // FCFS
    int time_quantum;                       // Time quantum for RR (ms)
    pthread_mutex_t scheduler_lock;
} MultilevelScheduler;

/**
 * Banker's Algorithm Data Structures
 */
typedef struct {
    int max_resources[3];           // Max resources: [RAM, HDD, Cores]
    int available[3];                // Available resources
    int allocation[MAX_PROCESSES][3]; // Currently allocated
    int max_need[MAX_PROCESSES][3];   // Maximum need
    int need[MAX_PROCESSES][3];       // Remaining need
} BankerState;

/**
 * IPC Message Structure
 */
typedef struct {
    long msg_type;
    char msg_text[256];
    int sender_pid;
    int data;
} IPCMessage;

/**
 * Thread Types
 */
typedef enum {
    THREAD_WORKER,      // Background task processing
    THREAD_UI,          // User interface handling
    THREAD_IO           // File I/O operations
} ThreadType;

/**
 * Application Descriptor
 */
typedef struct {
    int app_id;
    char name[MAX_NAME_LEN];
    AppCategory category;
    PriorityLevel priority;
    int memory_required;
    int hdd_required;
    int cpu_cores_needed;
    char executable_path[MAX_PATH_LEN];
} AppDescriptor;

/**
 * System Configuration
 */
typedef struct {
    char os_name[50];
    char os_version[20];
    int ram_size_mb;
    int hdd_size_gb;
    int cpu_cores;
    int rr_time_quantum_ms;
    int max_processes;
} SystemConfig;

#endif // TYPES_H
