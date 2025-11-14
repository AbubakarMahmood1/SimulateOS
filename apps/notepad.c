/**
 * notepad.c - Notepad+ Text Editor Application
 * Text editor with auto-save feature
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>

/**
 * main - Notepad+ entry point
 *
 * Simple text editor with simulated auto-save every 30 seconds.
 *
 * Returns: 0 on success
 */
int main(void) {
    char text[1000] = "";
    char line[256];

    printf("\n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("                       NOTEPAD+                             \n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("  PID: %d\n", getpid());
    printf("  Status: Running as separate process\n");
    printf("  Feature: Auto-save every 30 seconds\n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("\n");

    printf("Type your text (type 'save' to manually save, 'exit' to quit):\n");
    printf("\n");

    int save_counter = 0;
    while (1) {
        printf("> ");
        fgets(line, sizeof(line), stdin);
        line[strcspn(line, "\n")] = 0;

        if (strcmp(line, "exit") == 0) {
            break;
        } else if (strcmp(line, "save") == 0) {
            printf("[AUTO-SAVE] Document saved at %d seconds\n", save_counter);
        } else {
            strcat(text, line);
            strcat(text, "\n");
            save_counter += 5;  // Simulate time passing

            if (save_counter >= 30) {
                printf("[AUTO-SAVE] Document auto-saved!\n");
                save_counter = 0;
            }
        }
    }

    printf("Notepad+ closing...\n");
    return 0;
}
