/**
 * file_ops.c - File Operations Tool
 * Real file operations with progress tracking and threading
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>

#define BUFFER_SIZE 8192

typedef struct {
    char source[512];
    char dest[512];
    long total_size;
    long bytes_copied;
    int complete;
    int success;
} FileOp;

FileOp current_op;
pthread_mutex_t op_mutex;

/**
 * get_file_size - Get size of a file
 */
long get_file_size(const char *path) {
    struct stat st;
    if (stat(path, &st) == 0) {
        return st.st_size;
    }
    return -1;
}

/**
 * progress_thread - Display progress in separate thread
 */
void* progress_thread(void* arg) {
    (void)arg;

    while (1) {
        pthread_mutex_lock(&op_mutex);
        if (current_op.complete) {
            pthread_mutex_unlock(&op_mutex);
            break;
        }

        long total = current_op.total_size;
        long copied = current_op.bytes_copied;

        if (total > 0) {
            int percent = (copied * 100) / total;
            int bars = percent / 5;

            printf("\r  Progress: [");
            for (int i = 0; i < 20; i++) {
                if (i < bars) printf("█");
                else printf("░");
            }
            printf("] %3d%% (%ld/%ld bytes)", percent, copied, total);
            fflush(stdout);
        }

        pthread_mutex_unlock(&op_mutex);
        usleep(100000);  // Update every 0.1 seconds
    }

    return NULL;
}

/**
 * copy_file - Copy a file with progress tracking
 */
int copy_file(const char *source, const char *dest) {
    FILE *src = fopen(source, "rb");
    if (!src) {
        printf("Error: Cannot open source file: %s\n", source);
        return 0;
    }

    FILE *dst = fopen(dest, "wb");
    if (!dst) {
        printf("Error: Cannot create destination file: %s\n", dest);
        fclose(src);
        return 0;
    }

    // Initialize operation
    pthread_mutex_lock(&op_mutex);
    strcpy(current_op.source, source);
    strcpy(current_op.dest, dest);
    current_op.total_size = get_file_size(source);
    current_op.bytes_copied = 0;
    current_op.complete = 0;
    pthread_mutex_unlock(&op_mutex);

    // Start progress thread
    pthread_t prog_thread;
    pthread_create(&prog_thread, NULL, progress_thread, NULL);

    // Copy file
    char buffer[BUFFER_SIZE];
    size_t bytes;

    while ((bytes = fread(buffer, 1, BUFFER_SIZE, src)) > 0) {
        if (fwrite(buffer, 1, bytes, dst) != bytes) {
            printf("\nError: Write failed\n");
            fclose(src);
            fclose(dst);
            current_op.complete = 1;
            current_op.success = 0;
            pthread_join(prog_thread, NULL);
            return 0;
        }

        pthread_mutex_lock(&op_mutex);
        current_op.bytes_copied += bytes;
        pthread_mutex_unlock(&op_mutex);
    }

    fclose(src);
    fclose(dst);

    // Mark complete
    pthread_mutex_lock(&op_mutex);
    current_op.complete = 1;
    current_op.success = 1;
    pthread_mutex_unlock(&op_mutex);

    pthread_join(prog_thread, NULL);
    printf("\n");

    return 1;
}

/**
 * move_file - Move a file (copy then delete)
 */
int move_file(const char *source, const char *dest) {
    printf("\n[MOVE] %s → %s\n", source, dest);

    if (!copy_file(source, dest)) {
        return 0;
    }

    if (remove(source) == 0) {
        printf("  ✓ Source file deleted\n");
        return 1;
    } else {
        printf("  ✗ Failed to delete source file\n");
        return 0;
    }
}

/**
 * delete_file - Delete a file
 */
int delete_file(const char *path) {
    printf("\n[DELETE] %s\n", path);

    long size = get_file_size(path);
    if (size < 0) {
        printf("  ✗ File not found\n");
        return 0;
    }

    if (remove(path) == 0) {
        printf("  ✓ Deleted successfully (%ld bytes freed)\n", size);
        return 1;
    } else {
        printf("  ✗ Delete failed\n");
        return 0;
    }
}

/**
 * print_menu - Display main menu
 */
void print_menu(void) {
    printf("\n╔══════════════════════════════════════════════════════════════╗\n");
    printf("║                    FILE OPERATIONS MENU                      ║\n");
    printf("╠══════════════════════════════════════════════════════════════╣\n");
    printf("║  1. Copy file                                                ║\n");
    printf("║  2. Move file                                                ║\n");
    printf("║  3. Delete file                                              ║\n");
    printf("║  4. File information                                         ║\n");
    printf("║  5. Exit                                                     ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n");
    printf("\n");
}

/**
 * file_info - Display file information
 */
void file_info(const char *path) {
    struct stat st;
    if (stat(path, &st) != 0) {
        printf("Error: File not found: %s\n", path);
        return;
    }

    printf("\nFile Information: %s\n", path);
    printf("─────────────────────────────────────────────────────────────\n");
    printf("  Size:          %ld bytes (%.2f KB)\n", st.st_size, st.st_size / 1024.0);
    printf("  Permissions:   %o\n", st.st_mode & 0777);
    printf("  Type:          %s\n", S_ISDIR(st.st_mode) ? "Directory" : "File");
    printf("  Inode:         %ld\n", (long)st.st_ino);

    char time_str[64];
    struct tm *tm_info = localtime(&st.st_mtime);
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_info);
    printf("  Modified:      %s\n", time_str);
    printf("─────────────────────────────────────────────────────────────\n");
}

/**
 * main - File Operations Tool entry point
 *
 * Full-featured file operations with real copy/move/delete
 * and threaded progress tracking.
 *
 * Returns: 0 on success
 */
int main(void) {
    char source[512], dest[512];
    int choice;

    pthread_mutex_init(&op_mutex, NULL);

    printf("\n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("                  FILE OPERATIONS TOOL                      \n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("  PID: %d\n", getpid());
    printf("  Status: Running as REAL separate process\n");
    printf("  Features: Copy, Move, Delete with progress tracking\n");
    printf("════════════════════════════════════════════════════════════\n");

    while (1) {
        print_menu();

        printf("Select operation: ");
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');  // Clear input buffer
            printf("Invalid input\n");
            continue;
        }
        getchar();  // Consume newline

        if (choice == 5) {
            printf("\nExiting File Operations Tool...\n");
            break;
        }

        switch (choice) {
            case 1:  // Copy
                printf("\nSource file path: ");
                fgets(source, sizeof(source), stdin);
                source[strcspn(source, "\n")] = 0;

                printf("Destination path: ");
                fgets(dest, sizeof(dest), stdin);
                dest[strcspn(dest, "\n")] = 0;

                printf("\n[COPY] %s → %s\n", source, dest);
                if (copy_file(source, dest)) {
                    printf("  ✓ Copy completed successfully\n");
                } else {
                    printf("  ✗ Copy failed\n");
                }

                printf("\nPress Enter to continue...");
                getchar();
                break;

            case 2:  // Move
                printf("\nSource file path: ");
                fgets(source, sizeof(source), stdin);
                source[strcspn(source, "\n")] = 0;

                printf("Destination path: ");
                fgets(dest, sizeof(dest), stdin);
                dest[strcspn(dest, "\n")] = 0;

                if (move_file(source, dest)) {
                    printf("  ✓ Move completed successfully\n");
                } else {
                    printf("  ✗ Move failed\n");
                }

                printf("\nPress Enter to continue...");
                getchar();
                break;

            case 3:  // Delete
                printf("\nFile path to delete: ");
                fgets(source, sizeof(source), stdin);
                source[strcspn(source, "\n")] = 0;

                printf("Are you sure? (yes/no): ");
                fgets(dest, sizeof(dest), stdin);
                dest[strcspn(dest, "\n")] = 0;

                if (strcmp(dest, "yes") == 0) {
                    delete_file(source);
                } else {
                    printf("Delete cancelled\n");
                }

                printf("\nPress Enter to continue...");
                getchar();
                break;

            case 4:  // File info
                printf("\nFile path: ");
                fgets(source, sizeof(source), stdin);
                source[strcspn(source, "\n")] = 0;

                file_info(source);

                printf("\nPress Enter to continue...");
                getchar();
                break;

            default:
                printf("Invalid choice. Please select 1-5.\n");
                printf("\nPress Enter to continue...");
                getchar();
                break;
        }
    }

    pthread_mutex_destroy(&op_mutex);
    return 0;
}
