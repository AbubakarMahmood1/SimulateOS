/**
 * scheduler.c - Process scheduling system
 * Implements multilevel queue with Round Robin, Priority, and FCFS
 */

#include "../include/kernel.h"
#include "../include/types.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

/**
 * create_queue - Create a new process queue
 *
 * Allocates and initializes a new process queue with
 * proper synchronization primitives.
 *
 * Returns: Pointer to newly created ProcessQueue
 */
ProcessQueue* create_queue(void) {
    ProcessQueue* queue = (ProcessQueue*)malloc(sizeof(ProcessQueue));
    if (queue == NULL) {
        printf("[Scheduler] Error: Failed to allocate queue\n");
        return NULL;
    }

    queue->front = NULL;
    queue->rear = NULL;
    queue->size = 0;
    pthread_mutex_init(&queue->queue_lock, NULL);

    return queue;
}

/**
 * queue_enqueue - Add process to queue
 * @queue: Process queue
 * @process: Process to add
 *
 * Adds a process to the end of the queue.
 * Thread-safe using mutex.
 */
void queue_enqueue(ProcessQueue* queue, Process* process) {
    if (queue == NULL || process == NULL) {
        return;
    }

    pthread_mutex_lock(&queue->queue_lock);

    QueueNode* node = (QueueNode*)malloc(sizeof(QueueNode));
    node->process = process;
    node->next = NULL;

    if (queue->rear == NULL) {
        // Queue is empty
        queue->front = node;
        queue->rear = node;
    } else {
        // Add to rear
        queue->rear->next = node;
        queue->rear = node;
    }

    queue->size++;

    pthread_mutex_unlock(&queue->queue_lock);
}

/**
 * queue_dequeue - Remove process from queue
 * @queue: Process queue
 *
 * Removes and returns the process at the front of the queue.
 * Thread-safe using mutex.
 *
 * Returns: Pointer to Process, or NULL if queue empty
 */
Process* queue_dequeue(ProcessQueue* queue) {
    if (queue == NULL) {
        return NULL;
    }

    pthread_mutex_lock(&queue->queue_lock);

    if (queue->front == NULL) {
        pthread_mutex_unlock(&queue->queue_lock);
        return NULL;
    }

    QueueNode* node = queue->front;
    Process* process = node->process;

    queue->front = node->next;
    if (queue->front == NULL) {
        queue->rear = NULL;
    }

    queue->size--;

    free(node);

    pthread_mutex_unlock(&queue->queue_lock);

    return process;
}

/**
 * queue_is_empty - Check if queue is empty
 * @queue: Process queue
 *
 * Returns: true if queue is empty, false otherwise
 */
bool queue_is_empty(ProcessQueue* queue) {
    if (queue == NULL) {
        return true;
    }

    pthread_mutex_lock(&queue->queue_lock);
    bool empty = (queue->size == 0);
    pthread_mutex_unlock(&queue->queue_lock);

    return empty;
}

/**
 * destroy_queue - Destroy queue and free memory
 * @queue: Process queue to destroy
 *
 * Frees all nodes and the queue structure itself.
 */
void destroy_queue(ProcessQueue* queue) {
    if (queue == NULL) {
        return;
    }

    pthread_mutex_lock(&queue->queue_lock);

    QueueNode* current = queue->front;
    while (current != NULL) {
        QueueNode* next = current->next;
        free(current);
        current = next;
    }

    pthread_mutex_unlock(&queue->queue_lock);
    pthread_mutex_destroy(&queue->queue_lock);

    free(queue);
}

/**
 * init_scheduler - Initialize multilevel queue scheduler
 * @time_quantum: Time quantum for Round Robin in ms
 *
 * Creates and initializes a three-level queue scheduler:
 * Level 0: High priority (Round Robin)
 * Level 1: Medium priority (Priority Scheduling)
 * Level 2: Low priority (FCFS)
 *
 * Returns: Pointer to MultilevelScheduler
 */
MultilevelScheduler* init_scheduler(int time_quantum) {
    MultilevelScheduler* scheduler = (MultilevelScheduler*)malloc(sizeof(MultilevelScheduler));
    if (scheduler == NULL) {
        printf("[Scheduler] Error: Failed to allocate scheduler\n");
        return NULL;
    }

    scheduler->high_priority_queue = create_queue();
    scheduler->medium_priority_queue = create_queue();
    scheduler->low_priority_queue = create_queue();
    scheduler->time_quantum = time_quantum;
    pthread_mutex_init(&scheduler->scheduler_lock, NULL);

    printf("[Scheduler] Initialized multilevel queue (Time quantum: %d ms)\n", time_quantum);
    printf("            Level 0: High Priority (Round Robin)\n");
    printf("            Level 1: Medium Priority (Priority Scheduling)\n");
    printf("            Level 2: Low Priority (FCFS)\n");

    return scheduler;
}

/**
 * enqueue_process - Add process to appropriate queue
 * @scheduler: Multilevel scheduler
 * @process: Process to enqueue
 *
 * Adds process to the queue corresponding to its priority level.
 */
void enqueue_process(MultilevelScheduler* scheduler, Process* process) {
    if (scheduler == NULL || process == NULL) {
        return;
    }

    process->state = PROCESS_READY;

    switch (process->priority) {
        case PRIORITY_HIGH:
            queue_enqueue(scheduler->high_priority_queue, process);
            process->time_quantum_remaining = scheduler->time_quantum;
            printf("[Scheduler] Added PID=%d to HIGH priority queue (Round Robin)\n",
                   process->pid);
            break;

        case PRIORITY_MEDIUM:
            queue_enqueue(scheduler->medium_priority_queue, process);
            printf("[Scheduler] Added PID=%d to MEDIUM priority queue (Priority)\n",
                   process->pid);
            break;

        case PRIORITY_LOW:
            queue_enqueue(scheduler->low_priority_queue, process);
            printf("[Scheduler] Added PID=%d to LOW priority queue (FCFS)\n",
                   process->pid);
            break;
    }
}

/**
 * schedule_round_robin - Round Robin scheduling
 * @queue: Process queue
 * @time_quantum: Time quantum in ms
 *
 * Implements Round Robin scheduling algorithm.
 * Dequeues process, decrements time quantum, and re-enqueues
 * if time quantum not exhausted.
 *
 * Returns: Pointer to Process to run
 */
Process* schedule_round_robin(ProcessQueue* queue, int time_quantum) {
    Process* process = queue_dequeue(queue);

    if (process != NULL) {
        process->time_quantum_remaining = time_quantum;
        printf("[Scheduler] RR selected PID=%d (quantum=%d ms)\n",
               process->pid, time_quantum);
    }

    return process;
}

/**
 * schedule_priority - Priority scheduling
 * @queue: Process queue
 *
 * Implements priority scheduling.
 * For simplicity, uses FCFS within the priority level.
 *
 * Returns: Pointer to Process to run
 */
Process* schedule_priority(ProcessQueue* queue) {
    Process* process = queue_dequeue(queue);

    if (process != NULL) {
        printf("[Scheduler] Priority selected PID=%d\n", process->pid);
    }

    return process;
}

/**
 * schedule_fcfs - First-Come-First-Served scheduling
 * @queue: Process queue
 *
 * Implements FCFS scheduling algorithm.
 * Simply dequeues the first process.
 *
 * Returns: Pointer to Process to run
 */
Process* schedule_fcfs(ProcessQueue* queue) {
    Process* process = queue_dequeue(queue);

    if (process != NULL) {
        printf("[Scheduler] FCFS selected PID=%d\n", process->pid);
    }

    return process;
}

/**
 * get_next_process - Get next process to run
 * @scheduler: Multilevel scheduler
 *
 * Selects next process to run based on multilevel queue algorithm.
 * Checks queues in priority order: High -> Medium -> Low
 *
 * Returns: Pointer to next Process, or NULL if no processes ready
 */
Process* get_next_process(MultilevelScheduler* scheduler) {
    if (scheduler == NULL) {
        return NULL;
    }

    pthread_mutex_lock(&scheduler->scheduler_lock);

    Process* next = NULL;

    // Check high priority queue first (Round Robin)
    if (!queue_is_empty(scheduler->high_priority_queue)) {
        next = schedule_round_robin(scheduler->high_priority_queue,
                                    scheduler->time_quantum);
    }
    // Then medium priority queue (Priority Scheduling)
    else if (!queue_is_empty(scheduler->medium_priority_queue)) {
        next = schedule_priority(scheduler->medium_priority_queue);
    }
    // Finally low priority queue (FCFS)
    else if (!queue_is_empty(scheduler->low_priority_queue)) {
        next = schedule_fcfs(scheduler->low_priority_queue);
    }

    pthread_mutex_unlock(&scheduler->scheduler_lock);

    return next;
}

/**
 * context_switch - Perform context switch
 * @current: Currently running process
 * @next: Next process to run
 *
 * Simulates a context switch between processes.
 * In a real OS, this would save/restore CPU state.
 */
void context_switch(Process* current, Process* next) {
    if (current != NULL) {
        printf("[Scheduler] Context switch: PID=%d -> ", current->pid);
        current->state = PROCESS_READY;
    } else {
        printf("[Scheduler] Context switch: NULL -> ");
    }

    if (next != NULL) {
        printf("PID=%d\n", next->pid);
        next->state = PROCESS_RUNNING;
    } else {
        printf("NULL\n");
    }
}
