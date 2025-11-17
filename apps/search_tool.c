/**
 * search_tool.c - File Search Tool
 * Recursive file search with pattern matching
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>

int result_count = 0;

void search_directory(const char *path, const char *pattern, int max_depth, int current_depth) {
    if (current_depth > max_depth) return;
    
    DIR *dir = opendir(path);
    if (!dir) return;
    
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;
        
        char full_path[1024];
        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);
        
        struct stat st;
        if (stat(full_path, &st) == 0) {
            if (S_ISDIR(st.st_mode)) {
                search_directory(full_path, pattern, max_depth, current_depth + 1);
            } else if (strstr(entry->d_name, pattern) != NULL) {
                result_count++;
                printf("  [%d] %s (%ld bytes)\n", result_count, full_path, st.st_size);
            }
        }
    }
    closedir(dir);
}

int main(void) {
    char path[256] = ".";
    char pattern[100];
    char input[256];
    
    printf("\n════════════════════════════════════════════════════════════\n");
    printf("                      SEARCH TOOL                           \n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("  PID: %d\n", getpid());
    printf("  Status: Running as REAL separate process\n");
    printf("════════════════════════════════════════════════════════════\n\n");
    
    while (1) {
        printf("Enter search pattern (or 'exit'): ");
        fflush(stdout);
        if (fgets(pattern, sizeof(pattern), stdin) == NULL) break;
        pattern[strcspn(pattern, "\n")] = 0;
        
        if (strcmp(pattern, "exit") == 0) break;
        if (strlen(pattern) == 0) continue;
        
        printf("Search in directory [default: current]: ");
        fflush(stdout);
        if (fgets(input, sizeof(input), stdin) == NULL) break;
        input[strcspn(input, "\n")] = 0;
        if (strlen(input) > 0) {
            strncpy(path, input, sizeof(path) - 1);
        }
        
        printf("\nSearching for '%s' in %s...\n", pattern, path);
        printf("─────────────────────────────────────────────────────────────\n");
        
        result_count = 0;
        search_directory(path, pattern, 3, 0);
        
        printf("─────────────────────────────────────────────────────────────\n");
        printf("Found %d result(s)\n\n", result_count);
    }
    
    printf("\nSearch Tool closing...\n");
    return 0;
}
