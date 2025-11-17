/**
 * notepad.c - Notepad+ Text Editor Application
 * Text editor with REAL auto-save thread running every 30 seconds
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#define MAX_LINES 1000
#define MAX_LINE_LEN 256
#define AUTO_SAVE_INTERVAL 30  // seconds
#define SAVE_FILE "/tmp/minios_notepad.txt"

// Shared data
char* lines[MAX_LINES];
int line_count = 0;
pthread_mutex_t text_mutex;
int running = 1;
int modified = 0;

/**
 * save_to_file - Save all lines to file
 */
void save_to_file(void) {
    pthread_mutex_lock(&text_mutex);

    FILE* fp = fopen(SAVE_FILE, "w");
    if (fp == NULL) {
        pthread_mutex_unlock(&text_mutex);
        printf("\n[ERROR] Failed to open save file\n");
        return;
    }

    for (int i = 0; i < line_count; i++) {
        fprintf(fp, "%s\n", lines[i]);
    }

    fclose(fp);
    modified = 0;

    pthread_mutex_unlock(&text_mutex);
}

/**
 * load_from_file - Load text from file
 */
void load_from_file(void) {
    FILE* fp = fopen(SAVE_FILE, "r");
    if (fp == NULL) {
        return;  // File doesn't exist yet
    }

    char buffer[MAX_LINE_LEN];
    while (fgets(buffer, sizeof(buffer), fp) && line_count < MAX_LINES) {
        // Remove newline
        buffer[strcspn(buffer, "\n")] = 0;

        lines[line_count] = (char*)malloc(strlen(buffer) + 1);
        if (lines[line_count] != NULL) {
            strcpy(lines[line_count], buffer);
            line_count++;
        }
    }

    fclose(fp);
    printf("[LOAD] Loaded %d lines from %s\n", line_count, SAVE_FILE);
}

/**
 * auto_save_thread - Background thread for auto-save
 */
void* auto_save_thread(void* arg) {
    (void)arg;  // Unused

    printf("[AUTO-SAVE] Thread started (PID: %d, TID: %lu)\n",
           getpid(), pthread_self());

    int save_count = 0;

    while (running) {
        sleep(AUTO_SAVE_INTERVAL);

        if (!running) {
            break;
        }

        pthread_mutex_lock(&text_mutex);
        int needs_save = modified;
        pthread_mutex_unlock(&text_mutex);

        if (needs_save && line_count > 0) {
            save_count++;
            save_to_file();

            time_t now = time(NULL);
            struct tm* tm_info = localtime(&now);
            char time_str[64];
            strftime(time_str, sizeof(time_str), "%H:%M:%S", tm_info);

            printf("\n[AUTO-SAVE #%d] Document saved at %s\n",
                   save_count, time_str);
            printf("notepad> ");
            fflush(stdout);
        }
    }

    printf("[AUTO-SAVE] Thread exiting\n");
    return NULL;
}

/**
 * print_text - Display all lines
 */
void print_text(void) {
    pthread_mutex_lock(&text_mutex);

    printf("\n═══════════════════════════════════════════════════════════\n");
    printf("                    DOCUMENT CONTENT                        \n");
    printf("═══════════════════════════════════════════════════════════\n");

    if (line_count == 0) {
        printf("(empty document)\n");
    } else {
        for (int i = 0; i < line_count; i++) {
            printf("%3d: %s\n", i + 1, lines[i]);
        }
    }

    printf("═══════════════════════════════════════════════════════════\n");
    printf("Total lines: %d\n\n", line_count);

    pthread_mutex_unlock(&text_mutex);
}

/**
 * print_help - Display help information
 */
void print_help(void) {
    printf("\nAvailable Commands:\n");
    printf("  (text)  - Type text to add a new line\n");
    printf("  view    - View entire document\n");
    printf("  save    - Manually save document\n");
    printf("  clear   - Clear all text\n");
    printf("  help    - Show this help\n");
    printf("  exit    - Exit (auto-saves before exit)\n");
    printf("\nAuto-save runs every %d seconds in background thread\n\n",
           AUTO_SAVE_INTERVAL);
}

/**
 * main - Notepad+ entry point
 *
 * Full-featured text editor with pthread-based auto-save
 * running in a background thread every 30 seconds.
 *
 * Returns: 0 on success
 */
int main(void) {
    char input[MAX_LINE_LEN];
    pthread_t autosave_tid;

    printf("\n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("                       NOTEPAD+                             \n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("  PID: %d\n", getpid());
    printf("  Status: Running as REAL separate process\n");
    printf("  Feature: REAL auto-save thread every %d seconds\n", AUTO_SAVE_INTERVAL);
    printf("════════════════════════════════════════════════════════════\n");
    printf("\n");

    // Initialize mutex
    pthread_mutex_init(&text_mutex, NULL);

    // Load existing file if present
    load_from_file();

    // Start auto-save thread
    if (pthread_create(&autosave_tid, NULL, auto_save_thread, NULL) != 0) {
        fprintf(stderr, "Error: Failed to create auto-save thread\n");
        return 1;
    }

    printf("Notepad+ ready!\n");
    printf("Auto-save thread running (saves every %d seconds)\n", AUTO_SAVE_INTERVAL);
    print_help();

    while (1) {
        printf("notepad> ");
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }

        // Remove newline
        input[strcspn(input, "\n")] = 0;

        // Skip empty input
        if (strlen(input) == 0) {
            continue;
        }

        // Check for commands
        if (strcmp(input, "exit") == 0 || strcmp(input, "quit") == 0) {
            // Save before exiting
            if (modified) {
                printf("Saving before exit...\n");
                save_to_file();
            }
            break;
        }

        if (strcmp(input, "view") == 0) {
            print_text();
            continue;
        }

        if (strcmp(input, "save") == 0) {
            save_to_file();
            printf("[SAVE] Document saved to %s\n", SAVE_FILE);
            continue;
        }

        if (strcmp(input, "clear") == 0) {
            pthread_mutex_lock(&text_mutex);
            for (int i = 0; i < line_count; i++) {
                free(lines[i]);
            }
            line_count = 0;
            modified = 1;
            pthread_mutex_unlock(&text_mutex);
            printf("Document cleared.\n");
            continue;
        }

        if (strcmp(input, "help") == 0) {
            print_help();
            continue;
        }

        // Add line to document
        pthread_mutex_lock(&text_mutex);

        if (line_count >= MAX_LINES) {
            pthread_mutex_unlock(&text_mutex);
            printf("Error: Maximum line limit reached (%d lines)\n", MAX_LINES);
            continue;
        }

        lines[line_count] = (char*)malloc(strlen(input) + 1);
        if (lines[line_count] != NULL) {
            strcpy(lines[line_count], input);
            line_count++;
            modified = 1;
            printf("Line %d added.\n", line_count);
        } else {
            printf("Error: Memory allocation failed\n");
        }

        pthread_mutex_unlock(&text_mutex);
    }

    // Signal auto-save thread to exit
    running = 0;

    // Wait for auto-save thread to finish
    pthread_join(autosave_tid, NULL);

    // Cleanup
    pthread_mutex_destroy(&text_mutex);

    for (int i = 0; i < line_count; i++) {
        free(lines[i]);
    }

    printf("\nNotepad+ closing...\n");
    printf("Document saved to: %s\n", SAVE_FILE);

    return 0;
}
