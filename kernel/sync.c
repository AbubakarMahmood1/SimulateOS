/**
 * sync.c - Synchronization primitives
 * Implements mutexes, semaphores, and condition variables
 */

#include "../include/kernel.h"
#include "../include/types.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

/**
 * init_mutex - Initialize a mutex
 * @mutex: Pointer to mutex
 *
 * Initializes a mutex with default attributes.
 */
void init_mutex(pthread_mutex_t* mutex) {
    if (mutex == NULL) {
        return;
    }

    int result = pthread_mutex_init(mutex, NULL);
    if (result != 0) {
        printf("[Sync] Error initializing mutex: %d\n", result);
        return;
    }

    printf("[Sync] Mutex initialized\n");
}

/**
 * lock_mutex - Lock a mutex
 * @mutex: Pointer to mutex
 *
 * Acquires a mutex lock. Blocks if mutex is already locked.
 */
void lock_mutex(pthread_mutex_t* mutex) {
    if (mutex == NULL) {
        return;
    }

    int result = pthread_mutex_lock(mutex);
    if (result != 0) {
        printf("[Sync] Error locking mutex: %d\n", result);
        return;
    }
}

/**
 * unlock_mutex - Unlock a mutex
 * @mutex: Pointer to mutex
 *
 * Releases a mutex lock.
 */
void unlock_mutex(pthread_mutex_t* mutex) {
    if (mutex == NULL) {
        return;
    }

    int result = pthread_mutex_unlock(mutex);
    if (result != 0) {
        printf("[Sync] Error unlocking mutex: %d\n", result);
        return;
    }
}

/**
 * init_semaphore - Initialize a semaphore
 * @initial_value: Initial value of semaphore
 *
 * Creates and initializes a POSIX semaphore with the specified
 * initial value. Semaphore is shared between threads.
 *
 * Returns: Pointer to semaphore, or NULL on failure
 */
void* init_semaphore(int initial_value) {
    sem_t* sem = (sem_t*)malloc(sizeof(sem_t));
    if (sem == NULL) {
        printf("[Sync] Error allocating semaphore\n");
        return NULL;
    }

    // Initialize semaphore (0 = shared between threads, not processes)
    int result = sem_init(sem, 0, initial_value);
    if (result != 0) {
        printf("[Sync] Error initializing semaphore\n");
        free(sem);
        return NULL;
    }

    printf("[Sync] Semaphore initialized (value=%d)\n", initial_value);

    return (void*)sem;
}

/**
 * semaphore_wait - Wait on semaphore (P operation)
 * @sem: Pointer to semaphore
 *
 * Decrements semaphore value. Blocks if value is 0.
 */
void semaphore_wait(void* sem) {
    if (sem == NULL) {
        return;
    }

    sem_t* semaphore = (sem_t*)sem;

    int result = sem_wait(semaphore);
    if (result != 0) {
        printf("[Sync] Error waiting on semaphore\n");
        return;
    }
}

/**
 * semaphore_signal - Signal semaphore (V operation)
 * @sem: Pointer to semaphore
 *
 * Increments semaphore value, potentially waking a blocked thread.
 */
void semaphore_signal(void* sem) {
    if (sem == NULL) {
        return;
    }

    sem_t* semaphore = (sem_t*)sem;

    int result = sem_post(semaphore);
    if (result != 0) {
        printf("[Sync] Error signaling semaphore\n");
        return;
    }
}

/**
 * init_condition_var - Initialize condition variable
 * @cond: Pointer to condition variable
 *
 * Initializes a condition variable with default attributes.
 */
void init_condition_var(pthread_cond_t* cond) {
    if (cond == NULL) {
        return;
    }

    int result = pthread_cond_init(cond, NULL);
    if (result != 0) {
        printf("[Sync] Error initializing condition variable: %d\n", result);
        return;
    }

    printf("[Sync] Condition variable initialized\n");
}

/**
 * condition_wait - Wait on condition variable
 * @cond: Pointer to condition variable
 * @mutex: Pointer to associated mutex
 *
 * Atomically unlocks mutex and waits on condition variable.
 * Re-locks mutex before returning.
 */
void condition_wait(pthread_cond_t* cond, pthread_mutex_t* mutex) {
    if (cond == NULL || mutex == NULL) {
        return;
    }

    int result = pthread_cond_wait(cond, mutex);
    if (result != 0) {
        printf("[Sync] Error waiting on condition variable: %d\n", result);
        return;
    }
}

/**
 * condition_signal - Signal condition variable
 * @cond: Pointer to condition variable
 *
 * Wakes up one thread waiting on the condition variable.
 */
void condition_signal(pthread_cond_t* cond) {
    if (cond == NULL) {
        return;
    }

    int result = pthread_cond_signal(cond);
    if (result != 0) {
        printf("[Sync] Error signaling condition variable: %d\n", result);
        return;
    }
}

/**
 * condition_broadcast - Broadcast to all waiting threads
 * @cond: Pointer to condition variable
 *
 * Wakes up all threads waiting on the condition variable.
 */
void condition_broadcast(pthread_cond_t* cond) {
    if (cond == NULL) {
        return;
    }

    int result = pthread_cond_broadcast(cond);
    if (result != 0) {
        printf("[Sync] Error broadcasting condition variable: %d\n", result);
        return;
    }
}
