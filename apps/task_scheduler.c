/**
 * task_scheduler.c - Task Scheduler Application
 * Schedule commands/programs to run at specific times or intervals
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#define MAX_TASKS 20

typedef enum {
    TASK_ONCE,      // Run once at specified time
    TASK_INTERVAL   // Run repeatedly every N seconds
} TaskType;

typedef enum {
    STATUS_PENDING,
    STATUS_RUNNING,
    STATUS_COMPLETED,
    STATUS_CANCELLED
} TaskStatus;

typedef struct {
    int id;
    char name[100];
    char command[256];
    TaskType type;
    TaskStatus status;
    time_t execute_time;    // For TASK_ONCE
    int interval_seconds;   // For TASK_INTERVAL
    time_t last_run;
    int run_count;
    pthread_t thread;
} Task;

Task tasks[MAX_TASKS];
int task_count = 0;
int next_task_id = 1;
pthread_mutex_t tasks_mutex;
int scheduler_running = 1;

/**
 * execute_task - Execute a scheduled task
 */
void execute_task(Task* task) {
    printf("\n[EXECUTING] Task #%d: %s\n", task->id, task->name);
    printf("  Command: %s\n", task->command);

    int result = system(task->command);

    if (result == 0) {
        printf("  ✓ Task completed successfully\n");
    } else {
        printf("  ✗ Task failed with code %d\n", result);
    }

    task->last_run = time(NULL);
    task->run_count++;
}

/**
 * task_thread - Thread to execute a task
 */
void* task_thread(void* arg) {
    Task* task = (Task*)arg;

    while (scheduler_running && task->status != STATUS_CANCELLED) {
        time_t now = time(NULL);

        if (task->type == TASK_ONCE) {
            // Wait until execute time
            if (now >= task->execute_time) {
                task->status = STATUS_RUNNING;
                execute_task(task);
                task->status = STATUS_COMPLETED;
                break;
            }
            sleep(1);
        } else {  // TASK_INTERVAL
            // Check if it's time to run
            if (task->last_run == 0 || (now - task->last_run) >= task->interval_seconds) {
                task->status = STATUS_RUNNING;
                execute_task(task);
                task->status = STATUS_PENDING;
            }
            sleep(1);
        }
    }

    return NULL;
}

/**
 * add_task - Add a new task to the scheduler
 */
int add_task(const char* name, const char* command, TaskType type,
             time_t execute_time, int interval) {

    pthread_mutex_lock(&tasks_mutex);

    if (task_count >= MAX_TASKS) {
        pthread_mutex_unlock(&tasks_mutex);
        return -1;
    }

    Task* task = &tasks[task_count];
    task->id = next_task_id++;
    strncpy(task->name, name, sizeof(task->name) - 1);
    strncpy(task->command, command, sizeof(task->command) - 1);
    task->type = type;
    task->status = STATUS_PENDING;
    task->execute_time = execute_time;
    task->interval_seconds = interval;
    task->last_run = 0;
    task->run_count = 0;

    // Start task thread
    pthread_create(&task->thread, NULL, task_thread, task);

    task_count++;

    pthread_mutex_unlock(&tasks_mutex);

    return task->id;
}

/**
 * list_tasks - Display all tasks
 */
void list_tasks(void) {
    const char* status_str[] = {"PENDING", "RUNNING", "COMPLETED", "CANCELLED"};
    const char* type_str[] = {"ONCE", "INTERVAL"};

    printf("\n╔════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                           SCHEDULED TASKS                              ║\n");
    printf("╠════════════════════════════════════════════════════════════════════════╣\n");
    printf("║ ID  │ Name              │ Type     │ Status     │ Runs │ Next/Int    ║\n");
    printf("║─────────────────────────────────────────────────────────────────────────║\n");

    pthread_mutex_lock(&tasks_mutex);

    if (task_count == 0) {
        printf("║ No tasks scheduled                                                     ║\n");
    } else {
        time_t now = time(NULL);
        for (int i = 0; i < task_count; i++) {
            Task* t = &tasks[i];
            char info[20];

            if (t->type == TASK_ONCE) {
                int seconds_until = (int)difftime(t->execute_time, now);
                if (seconds_until > 0) {
                    snprintf(info, sizeof(info), "in %ds", seconds_until);
                } else {
                    snprintf(info, sizeof(info), "due");
                }
            } else {
                snprintf(info, sizeof(info), "every %ds", t->interval_seconds);
            }

            printf("║ %-3d │ %-17s│ %-8s│ %-10s│ %-4d│ %-11s║\n",
                   t->id, t->name, type_str[t->type], status_str[t->status],
                   t->run_count, info);
        }
    }

    pthread_mutex_unlock(&tasks_mutex);

    printf("╚════════════════════════════════════════════════════════════════════════╝\n\n");
}

/**
 * cancel_task - Cancel a task by ID
 */
int cancel_task(int task_id) {
    pthread_mutex_lock(&tasks_mutex);

    for (int i = 0; i < task_count; i++) {
        if (tasks[i].id == task_id) {
            tasks[i].status = STATUS_CANCELLED;
            pthread_mutex_unlock(&tasks_mutex);
            printf("Task #%d cancelled\n", task_id);
            return 1;
        }
    }

    pthread_mutex_unlock(&tasks_mutex);
    printf("Task #%d not found\n", task_id);
    return 0;
}

/**
 * main - Task Scheduler entry point
 *
 * Full-featured task scheduler with one-time and interval tasks,
 * threaded execution, and real command execution.
 *
 * Returns: 0 on success
 */
int main(void) {
    char input[512];
    char name[100], command[256];
    int choice, interval, task_id;

    pthread_mutex_init(&tasks_mutex, NULL);

    printf("\n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("                     TASK SCHEDULER                         \n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("  PID: %d\n", getpid());
    printf("  Status: Running as REAL separate process\n");
    printf("  Features: Schedule commands with timers and threads\n");
    printf("════════════════════════════════════════════════════════════\n");

    // Add some example tasks
    add_task("Print Date", "date", TASK_INTERVAL, 0, 10);
    add_task("List Files", "ls /tmp", TASK_ONCE, time(NULL) + 5, 0);

    while (1) {
        list_tasks();

        printf("Options:\n");
        printf("  1. Add one-time task\n");
        printf("  2. Add interval task\n");
        printf("  3. Cancel task\n");
        printf("  4. Refresh\n");
        printf("  5. Exit\n");
        printf("\nselect> ");
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL) break;

        if (sscanf(input, "%d", &choice) != 1) continue;

        switch (choice) {
            case 1:  // One-time task
                printf("\nTask name: ");
                fgets(name, sizeof(name), stdin);
                name[strcspn(name, "\n")] = 0;

                printf("Command to execute: ");
                fgets(command, sizeof(command), stdin);
                command[strcspn(command, "\n")] = 0;

                printf("Delay in seconds: ");
                scanf("%d", &interval);
                getchar();

                task_id = add_task(name, command, TASK_ONCE,
                                  time(NULL) + interval, 0);

                if (task_id > 0) {
                    printf("✓ Task #%d scheduled to run in %d seconds\n",
                           task_id, interval);
                } else {
                    printf("✗ Failed to add task (max %d tasks)\n", MAX_TASKS);
                }

                printf("\nPress Enter to continue...");
                getchar();
                break;

            case 2:  // Interval task
                printf("\nTask name: ");
                fgets(name, sizeof(name), stdin);
                name[strcspn(name, "\n")] = 0;

                printf("Command to execute: ");
                fgets(command, sizeof(command), stdin);
                command[strcspn(command, "\n")] = 0;

                printf("Interval in seconds: ");
                scanf("%d", &interval);
                getchar();

                task_id = add_task(name, command, TASK_INTERVAL, 0, interval);

                if (task_id > 0) {
                    printf("✓ Task #%d scheduled to run every %d seconds\n",
                           task_id, interval);
                } else {
                    printf("✗ Failed to add task (max %d tasks)\n", MAX_TASKS);
                }

                printf("\nPress Enter to continue...");
                getchar();
                break;

            case 3:  // Cancel task
                printf("\nTask ID to cancel: ");
                scanf("%d", &task_id);
                getchar();

                cancel_task(task_id);

                printf("\nPress Enter to continue...");
                getchar();
                break;

            case 4:  // Refresh
                break;

            case 5:  // Exit
                scheduler_running = 0;

                // Wait for all task threads to finish
                pthread_mutex_lock(&tasks_mutex);
                for (int i = 0; i < task_count; i++) {
                    tasks[i].status = STATUS_CANCELLED;
                    pthread_join(tasks[i].thread, NULL);
                }
                pthread_mutex_unlock(&tasks_mutex);

                printf("\nTask Scheduler shutting down...\n");
                pthread_mutex_destroy(&tasks_mutex);
                return 0;

            default:
                printf("Invalid choice\n");
                printf("\nPress Enter to continue...");
                getchar();
        }
    }

    return 0;
}
