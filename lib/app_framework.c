/**
 * app_framework.c - Application framework and launcher
 * Handles launching and managing applications
 */

#include "../include/kernel.h"
#include "../include/types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

// Application descriptors for all available apps
static AppDescriptor available_apps[] = {
    // System Applications
    {1, "System Monitor", APP_SYSTEM, PRIORITY_HIGH, 64, 10, 1, "./apps/system_monitor"},
    {2, "Process Manager", APP_SYSTEM, PRIORITY_HIGH, 32, 5, 1, "./apps/process_manager"},
    {3, "File Explorer", APP_SYSTEM, PRIORITY_MEDIUM, 128, 20, 1, "./apps/file_explorer"},
    {4, "Terminal", APP_SYSTEM, PRIORITY_HIGH, 64, 10, 1, "./apps/terminal"},
    {5, "Settings", APP_SYSTEM, PRIORITY_LOW, 32, 5, 1, "./apps/settings"},

    // Productivity Applications
    {6, "Notepad+", APP_PRODUCTIVITY, PRIORITY_HIGH, 64, 50, 1, "./apps/notepad"},
    {7, "Calculator", APP_PRODUCTIVITY, PRIORITY_HIGH, 32, 5, 1, "./apps/calculator"},
    {8, "Calendar", APP_PRODUCTIVITY, PRIORITY_MEDIUM, 48, 10, 1, "./apps/calendar"},
    {9, "Task Scheduler", APP_PRODUCTIVITY, PRIORITY_MEDIUM, 48, 10, 1, "./apps/task_scheduler"},

    // Utility Applications
    {10, "File Operations", APP_UTILITY, PRIORITY_MEDIUM, 64, 20, 1, "./apps/file_ops"},
    {11, "System Info", APP_UTILITY, PRIORITY_LOW, 32, 5, 1, "./apps/system_info"},
    {12, "Search Tool", APP_UTILITY, PRIORITY_MEDIUM, 64, 15, 1, "./apps/search_tool"},

    // Entertainment Applications
    {13, "Minesweeper", APP_ENTERTAINMENT, PRIORITY_HIGH, 64, 10, 1, "./apps/minesweeper"},
    {14, "Music Player", APP_ENTERTAINMENT, PRIORITY_MEDIUM, 96, 100, 1, "./apps/music_player"},
    {15, "Clock & Timer", APP_ENTERTAINMENT, PRIORITY_LOW, 32, 5, 1, "./apps/clock"}
};

static int num_apps = sizeof(available_apps) / sizeof(AppDescriptor);

/**
 * get_available_apps - Get list of available applications
 * @apps: Array to fill with application descriptors
 * @max_apps: Maximum number of apps to return
 *
 * Copies available application descriptors to the provided array.
 *
 * Returns: Number of applications copied
 */
int get_available_apps(AppDescriptor apps[], int max_apps) {
    int count = (num_apps < max_apps) ? num_apps : max_apps;

    for (int i = 0; i < count; i++) {
        apps[i] = available_apps[i];
    }

    return count;
}

/**
 * find_app_by_id - Find application descriptor by ID
 * @app_id: Application ID
 *
 * Returns: Pointer to AppDescriptor, or NULL if not found
 */
static AppDescriptor* find_app_by_id(int app_id) {
    for (int i = 0; i < num_apps; i++) {
        if (available_apps[i].app_id == app_id) {
            return &available_apps[i];
        }
    }
    return NULL;
}

/**
 * check_command_exists - Check if a command exists in PATH
 * @command: Command to check
 *
 * Returns: 1 if command exists, 0 otherwise
 */
static int check_command_exists(const char* command) {
    char check_cmd[256];
    snprintf(check_cmd, sizeof(check_cmd), "command -v %s >/dev/null 2>&1", command);
    return system(check_cmd) == 0;
}

/**
 * launch_in_tmux - Launch application in new tmux window
 * @app: Application descriptor
 *
 * Returns: PID of child process, or -1 on failure
 */
static pid_t launch_in_tmux(AppDescriptor* app) {
    char tmux_cmd[1024];
    pid_t pid;

    // Create a wrapper script to capture the PID
    snprintf(tmux_cmd, sizeof(tmux_cmd),
             "tmux new-window -n '%s' '%s; echo; echo \"[Process finished - Press Enter to close]\"; read'",
             app->name, app->executable_path);

    pid = fork();
    if (pid == 0) {
        // Child: execute tmux command
        setsid();  // Create new session to detach from parent
        execl("/bin/sh", "sh", "-c", tmux_cmd, NULL);
        exit(1);
    }

    if (pid > 0) {
        // Give tmux a moment to start the window
        usleep(100000);  // 100ms
        printf("[Terminal] Launched in tmux window: %s\n", app->name);
    }

    return pid;
}

/**
 * launch_in_xterm - Launch application in xterm
 * @app: Application descriptor
 *
 * Returns: PID of child process, or -1 on failure
 */
static pid_t launch_in_xterm(AppDescriptor* app) {
    pid_t pid = fork();

    if (pid == 0) {
        // Child: launch xterm
        setsid();  // Create new session
        execlp("xterm", "xterm",
               "-T", app->name,
               "-e", "sh", "-c",
               app->executable_path,
               NULL);
        exit(1);
    }

    if (pid > 0) {
        printf("[Terminal] Launched in xterm: %s\n", app->name);
    }

    return pid;
}

/**
 * launch_in_gnome_terminal - Launch application in gnome-terminal
 * @app: Application descriptor
 *
 * Returns: PID of child process, or -1 on failure
 */
static pid_t launch_in_gnome_terminal(AppDescriptor* app) {
    pid_t pid = fork();

    if (pid == 0) {
        // Child: launch gnome-terminal
        setsid();
        execlp("gnome-terminal", "gnome-terminal",
               "--title", app->name,
               "--", app->executable_path,
               NULL);
        exit(1);
    }

    if (pid > 0) {
        printf("[Terminal] Launched in gnome-terminal: %s\n", app->name);
    }

    return pid;
}

/**
 * launch_direct - Launch application directly (fallback)
 * @app: Application descriptor
 *
 * Returns: PID of child process, or -1 on failure
 */
static pid_t launch_direct(AppDescriptor* app) {
    pid_t pid = fork();

    if (pid == 0) {
        // Child: execute directly
        // Redirect output to /dev/null since no terminal available
        freopen("/dev/null", "w", stdout);
        freopen("/dev/null", "w", stderr);

        execl(app->executable_path, app->name, NULL);
        exit(1);
    }

    if (pid > 0) {
        printf("[Terminal] Launched in background (no terminal available): %s\n", app->name);
        printf("           [Note: Output redirected to /dev/null]\n");
    }

    return pid;
}

/**
 * launch_in_terminal - Launch application in a separate terminal window
 * @app: Application descriptor
 * @process: Process structure
 *
 * Tries multiple methods to launch the application in a separate terminal:
 * 1. tmux (if we're in a tmux session)
 * 2. xterm (if DISPLAY is set and xterm is available)
 * 3. gnome-terminal (if available)
 * 4. Direct execution (fallback - no separate window)
 *
 * Returns: PID of the launched process, or -1 on failure
 */
static pid_t launch_in_terminal(AppDescriptor* app, Process* process) {
    (void)process;  // Unused for now
    pid_t child_pid = -1;

    // Method 1: Try tmux if we're in a tmux session
    if (getenv("TMUX") != NULL) {
        printf("[Terminal] Detected tmux session, creating new window...\n");
        child_pid = launch_in_tmux(app);
        if (child_pid > 0) {
            return child_pid;
        }
        printf("[Terminal] tmux launch failed, trying next method...\n");
    }

    // Method 2: Try xterm if X11 is available
    if (getenv("DISPLAY") != NULL && check_command_exists("xterm")) {
        printf("[Terminal] Detected X11 display, using xterm...\n");
        child_pid = launch_in_xterm(app);
        if (child_pid > 0) {
            return child_pid;
        }
        printf("[Terminal] xterm launch failed, trying next method...\n");
    }

    // Method 3: Try gnome-terminal
    if (check_command_exists("gnome-terminal")) {
        printf("[Terminal] Detected gnome-terminal, using it...\n");
        child_pid = launch_in_gnome_terminal(app);
        if (child_pid > 0) {
            return child_pid;
        }
        printf("[Terminal] gnome-terminal launch failed, trying next method...\n");
    }

    // Method 4: Fallback to direct execution
    printf("[Terminal] No terminal emulator available, launching in background...\n");
    child_pid = launch_direct(app);

    if (child_pid == -1) {
        perror("[Terminal] All launch methods failed");
    }

    return child_pid;
}

/**
 * launch_application - Launch an application as a separate process
 * @app_name: Name of the application
 * @app: Application descriptor
 * @resources: System resources
 * @scheduler: Scheduler
 * @banker: Banker's algorithm state
 *
 * Launches an application by:
 * 1. Checking resources with Banker's Algorithm
 * 2. Creating a process entry
 * 3. Forking a new process
 * 4. Adding to scheduler queue
 *
 * Returns: PID of launched app, or -1 on failure
 */
int launch_application(const char* app_name, AppDescriptor* app,
                      SystemResources* resources, MultilevelScheduler* scheduler,
                      BankerState* banker) {
    (void)app_name;  // Parameter kept for API consistency

    if (app == NULL) {
        printf("[App Launcher] Error: Invalid application\n");
        return -1;
    }

    printf("\n[App Launcher] Launching: %s\n", app->name);

    // Check resources using Banker's Algorithm
    if (!request_resources(banker, app->app_id, app->memory_required,
                          app->hdd_required, app->cpu_cores_needed)) {
        printf("[App Launcher] Failed: Insufficient resources (Banker's check)\n");
        return -1;
    }

    // Allocate memory
    if (!allocate_memory(resources, app->memory_required, app->hdd_required)) {
        release_resources(banker, app->app_id, app->memory_required,
                         app->hdd_required, app->cpu_cores_needed);
        printf("[App Launcher] Failed: Memory allocation failed\n");
        return -1;
    }

    // Create process entry
    Process* process = create_process(app->name, app->priority,
                                     app->memory_required, app->hdd_required,
                                     app->cpu_cores_needed, resources);

    if (process == NULL) {
        deallocate_memory(resources, app->memory_required, app->hdd_required);
        release_resources(banker, app->app_id, app->memory_required,
                         app->hdd_required, app->cpu_cores_needed);
        printf("[App Launcher] Failed: Process creation failed\n");
        return -1;
    }

    // Add to scheduler
    enqueue_process(scheduler, process);

    // Launch application in separate terminal window
    pid_t child_pid = launch_in_terminal(app, process);

    if (child_pid == -1) {
        // Cleanup on launch failure
        deallocate_memory(resources, app->memory_required, app->hdd_required);
        release_resources(banker, app->app_id, app->memory_required,
                         app->hdd_required, app->cpu_cores_needed);
        terminate_process(process, resources);
        return -1;
    }

    // Parent process - track the child
    process->system_pid = child_pid;
    update_process_state(process, PROCESS_RUNNING);

    printf("[App Launcher] Successfully launched: %s (PID=%d, System PID=%d)\n",
           app->name, process->pid, child_pid);

    return process->pid;
}

/**
 * launch_app_by_id - Launch application by ID
 * @app_id: Application ID
 * @resources: System resources
 * @scheduler: Scheduler
 * @banker: Banker's algorithm state
 *
 * Convenience function to launch an app by its ID.
 *
 * Returns: PID of launched app, or -1 on failure
 */
int launch_app_by_id(int app_id, SystemResources* resources,
                    MultilevelScheduler* scheduler, BankerState* banker) {
    AppDescriptor* app = find_app_by_id(app_id);
    if (app == NULL) {
        printf("[App Launcher] Error: Application ID %d not found\n", app_id);
        return -1;
    }

    return launch_application(app->name, app, resources, scheduler, banker);
}

/**
 * kill_application - Kill an application by PID
 * @pid: Process ID
 * @resources: System resources
 *
 * Terminates an application and frees its resources.
 *
 * Returns: true on success, false on failure
 */
bool kill_application(int pid, SystemResources* resources) {
    Process* process = find_process_by_pid(resources, pid);

    if (process == NULL) {
        printf("[App Launcher] Error: Process %d not found\n", pid);
        return false;
    }

    printf("[App Launcher] Terminating process: %s (PID=%d, System PID=%d)\n",
           process->name, pid, process->system_pid);

    // Kill the actual system process if it exists
    if (process->system_pid > 0) {
        if (kill(process->system_pid, SIGTERM) == 0) {
            printf("[App Launcher] Sent SIGTERM to system PID %d\n", process->system_pid);
            // Wait for process to terminate
            waitpid(process->system_pid, NULL, WNOHANG);
        } else {
            perror("[App Launcher] Failed to kill process");
        }
    }

    // Free memory
    deallocate_memory(resources, process->memory_required, process->hdd_required);

    // Terminate process
    terminate_process(process, resources);

    return true;
}

/**
 * print_available_apps - Print list of available applications
 *
 * Displays all applications organized by category.
 */
void print_available_apps(void) {
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                           AVAILABLE APPLICATIONS                           ║\n");
    printf("╠════════════════════════════════════════════════════════════════════════════╣\n");

    const char* categories[] = {"SYSTEM", "PRODUCTIVITY", "UTILITY", "ENTERTAINMENT"};

    for (int cat = 0; cat < 4; cat++) {
        printf("║                                                                            ║\n");
        printf("║  %-74s║\n", categories[cat]);
        printf("║  %-74s║\n", "──────────────────────────────────────");

        for (int i = 0; i < num_apps; i++) {
            if (available_apps[i].category == (AppCategory)cat) {
                printf("║  %2d. %-40s  RAM:%4dMB HDD:%4dMB      ║\n",
                       available_apps[i].app_id,
                       available_apps[i].name,
                       available_apps[i].memory_required,
                       available_apps[i].hdd_required);
            }
        }
    }

    printf("║                                                                            ║\n");
    printf("╚════════════════════════════════════════════════════════════════════════════╝\n");
    printf("\n");
}
