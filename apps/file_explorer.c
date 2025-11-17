/**
 * file_explorer.c - File Explorer Application
 * Navigate directories and view file information
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

char current_path[1024];

void list_directory(const char *path) {
    DIR *dir = opendir(path);
    if (!dir) {
        printf("Error: Cannot open directory\n");
        return;
    }
    
    printf("\nCurrent Directory: %s\n", path);
    printf("═══════════════════════════════════════════════════════════\n");
    printf("%-20s %10s %12s %s\n", "Name", "Type", "Size", "Modified");
    printf("───────────────────────────────────────────────────────────\n");
    
    struct dirent *entry;
    struct stat st;
    char full_path[2048];
    
    while ((entry = readdir(dir)) != NULL) {
        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);
        
        if (stat(full_path, &st) == 0) {
            char *type = S_ISDIR(st.st_mode) ? "[DIR]" : "[FILE]";
            char size_str[20];
            char time_str[20];
            
            if (S_ISDIR(st.st_mode)) {
                strcpy(size_str, "-");
            } else {
                snprintf(size_str, sizeof(size_str), "%ld", st.st_size);
            }
            
            struct tm *tm_info = localtime(&st.st_mtime);
            strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M", tm_info);
            
            printf("%-20s %10s %12s %s\n", entry->d_name, type, size_str, time_str);
        }
    }
    printf("═══════════════════════════════════════════════════════════\n\n");
    closedir(dir);
}

int main(void) {
    char input[256];
    char command[50];
    char arg[256];
    
    getcwd(current_path, sizeof(current_path));
    
    printf("\n════════════════════════════════════════════════════════════\n");
    printf("                    FILE EXPLORER                           \n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("  PID: %d\n", getpid());
    printf("  Status: Running as REAL separate process\n");
    printf("════════════════════════════════════════════════════════════\n\n");
    
    while (1) {
        list_directory(current_path);
        
        printf("Commands: cd <dir>, up, home, pwd, exit\n");
        printf("explorer> ");
        fflush(stdout);
        
        if (fgets(input, sizeof(input), stdin) == NULL) break;
        input[strcspn(input, "\n")] = 0;
        
        if (strlen(input) == 0) continue;
        
        sscanf(input, "%s %s", command, arg);
        
        if (strcmp(command, "exit") == 0) break;
        
        if (strcmp(command, "cd") == 0) {
            char new_path[2048];
            snprintf(new_path, sizeof(new_path), "%s/%s", current_path, arg);
            
            DIR *test = opendir(new_path);
            if (test) {
                closedir(test);
                realpath(new_path, current_path);
            } else {
                printf("Error: Directory not found\n");
            }
        } else if (strcmp(command, "up") == 0) {
            char *last_slash = strrchr(current_path, '/');
            if (last_slash && last_slash != current_path) {
                *last_slash = '\0';
            }
        } else if (strcmp(command, "home") == 0) {
            strcpy(current_path, getenv("HOME"));
        } else if (strcmp(command, "pwd") == 0) {
            printf("%s\n", current_path);
        }
    }
    
    printf("\nFile Explorer closing...\n");
    return 0;
}
