/**
 * ipc.c - Inter-Process Communication mechanisms
 * Implements shared memory, message queues, and pipes
 */

#include "../include/kernel.h"
#include "../include/types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <unistd.h>
#include <errno.h>

/**
 * init_ipc - Initialize IPC system
 *
 * Creates a message queue for inter-process communication.
 * Uses a unique key based on current directory and project ID.
 *
 * Returns: Message queue ID, or -1 on failure
 */
int init_ipc(void) {
    key_t key;

    // Generate unique key
    key = ftok(".", 'M');  // 'M' for MiniOS
    if (key == -1) {
        perror("[IPC] Error generating key");
        return -1;
    }

    // Create message queue
    int msg_queue_id = msgget(key, 0666 | IPC_CREAT);
    if (msg_queue_id == -1) {
        perror("[IPC] Error creating message queue");
        return -1;
    }

    printf("[IPC] Message queue created (ID: %d)\n", msg_queue_id);

    return msg_queue_id;
}

/**
 * send_message - Send message via message queue
 * @msg_queue_id: Message queue ID
 * @msg: Pointer to message structure
 *
 * Sends a message through the specified message queue.
 * Non-blocking send operation.
 *
 * Returns: true on success, false on failure
 */
bool send_message(int msg_queue_id, IPCMessage* msg) {
    if (msg == NULL) {
        return false;
    }

    // Send message (non-blocking)
    int result = msgsnd(msg_queue_id, msg, sizeof(IPCMessage) - sizeof(long),
                       IPC_NOWAIT);

    if (result == -1) {
        if (errno != EAGAIN) {
            perror("[IPC] Error sending message");
        }
        return false;
    }

    printf("[IPC] Message sent: Type=%ld, From PID=%d\n",
           msg->msg_type, msg->sender_pid);

    return true;
}

/**
 * receive_message - Receive message from message queue
 * @msg_queue_id: Message queue ID
 * @msg: Pointer to message structure to fill
 * @msg_type: Type of message to receive (0 for any type)
 *
 * Receives a message from the specified message queue.
 * Non-blocking receive operation.
 *
 * Returns: true on success, false on failure or no message
 */
bool receive_message(int msg_queue_id, IPCMessage* msg, long msg_type) {
    if (msg == NULL) {
        return false;
    }

    // Receive message (non-blocking)
    int result = msgrcv(msg_queue_id, msg, sizeof(IPCMessage) - sizeof(long),
                       msg_type, IPC_NOWAIT);

    if (result == -1) {
        if (errno != ENOMSG && errno != EAGAIN) {
            perror("[IPC] Error receiving message");
        }
        return false;
    }

    printf("[IPC] Message received: Type=%ld, From PID=%d\n",
           msg->msg_type, msg->sender_pid);

    return true;
}

/**
 * create_shared_memory - Create shared memory segment
 * @size: Size of shared memory segment in bytes
 *
 * Creates a new shared memory segment with the specified size.
 * Memory is readable and writable by all processes.
 *
 * Returns: Shared memory ID, or -1 on failure
 */
int create_shared_memory(size_t size) {
    key_t key;

    // Generate unique key
    key = ftok(".", 'S');  // 'S' for Shared Memory
    if (key == -1) {
        perror("[IPC] Error generating shared memory key");
        return -1;
    }

    // Create shared memory segment
    int shm_id = shmget(key, size, 0666 | IPC_CREAT);
    if (shm_id == -1) {
        perror("[IPC] Error creating shared memory");
        return -1;
    }

    printf("[IPC] Shared memory created (ID: %d, Size: %zu bytes)\n",
           shm_id, size);

    return shm_id;
}

/**
 * attach_shared_memory - Attach to shared memory segment
 * @shm_id: Shared memory ID
 *
 * Attaches to the specified shared memory segment.
 *
 * Returns: Pointer to shared memory, or NULL on failure
 */
void* attach_shared_memory(int shm_id) {
    void* shm_ptr = shmat(shm_id, NULL, 0);

    if (shm_ptr == (void*)-1) {
        perror("[IPC] Error attaching to shared memory");
        return NULL;
    }

    printf("[IPC] Attached to shared memory (ID: %d)\n", shm_id);

    return shm_ptr;
}

/**
 * detach_shared_memory - Detach from shared memory segment
 * @shm_ptr: Pointer to shared memory
 *
 * Detaches from the specified shared memory segment.
 */
void detach_shared_memory(void* shm_ptr) {
    if (shm_ptr == NULL) {
        return;
    }

    if (shmdt(shm_ptr) == -1) {
        perror("[IPC] Error detaching from shared memory");
        return;
    }

    printf("[IPC] Detached from shared memory\n");
}

/**
 * create_pipe - Create a pipe for communication
 * @pipe_fd: Array to store pipe file descriptors [read_fd, write_fd]
 *
 * Creates a unidirectional pipe for inter-process communication.
 * pipe_fd[0] is the read end, pipe_fd[1] is the write end.
 *
 * Returns: true on success, false on failure
 */
bool create_pipe(int pipe_fd[2]) {
    if (pipe(pipe_fd) == -1) {
        perror("[IPC] Error creating pipe");
        return false;
    }

    printf("[IPC] Pipe created (Read FD: %d, Write FD: %d)\n",
           pipe_fd[0], pipe_fd[1]);

    return true;
}
