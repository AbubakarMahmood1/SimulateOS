/**
 * search_tool.c - Search Tool Application
 * File and content search utility
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>

/**
 * main - Search Tool entry point
 *
 * Provides file name and content search functionality.
 *
 * Returns: 0 on success
 */
int main(void) {
    char query[256];

    printf("\n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("                      SEARCH TOOL                           \n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("  PID: %d\n", getpid());
    printf("  Status: Running as separate process\n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("\n");

    printf("Enter search query (or 'exit' to quit): ");
    fgets(query, sizeof(query), stdin);
    query[strcspn(query, "\n")] = 0;

    if (strcmp(query, "exit") == 0) {
        printf("Search Tool closing...\n");
        return 0;
    }

    printf("\nSearching for: '%s'\n", query);
    printf("─────────────────────────────────────────────────────────────\n");
    sleep(1);

    printf("\nSearch Results:\n");
    printf("─────────────────────────────────────────────────────────────\n");
    printf("  [FILE] /Documents/report_%s.pdf (2.5 MB)\n", query);
    printf("  [FILE] /Downloads/%s_data.csv (150 KB)\n", query);
    printf("  [DIR]  /Projects/%s_project/\n", query);
    printf("  [FILE] /Notes/%s_notes.txt (5 KB)\n", query);
    printf("─────────────────────────────────────────────────────────────\n");
    printf("\n");
    printf("Found 4 results in 0.25 seconds\n");
    printf("\n");

    printf("Press Enter to close...");
    getchar();

    return 0;
}
