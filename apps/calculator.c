/**
 * calculator.c - Scientific Calculator Application
 * Full-featured calculator with history and continuous operation
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>

#define MAX_HISTORY 20

typedef struct {
    double num1;
    double num2;
    char op;
    double result;
} HistoryEntry;

HistoryEntry history[MAX_HISTORY];
int history_count = 0;

/**
 * add_to_history - Add calculation to history
 */
void add_to_history(double num1, char op, double num2, double result) {
    if (history_count < MAX_HISTORY) {
        history[history_count].num1 = num1;
        history[history_count].op = op;
        history[history_count].num2 = num2;
        history[history_count].result = result;
        history_count++;
    } else {
        // Shift history
        for (int i = 0; i < MAX_HISTORY - 1; i++) {
            history[i] = history[i + 1];
        }
        history[MAX_HISTORY - 1].num1 = num1;
        history[MAX_HISTORY - 1].op = op;
        history[MAX_HISTORY - 1].num2 = num2;
        history[MAX_HISTORY - 1].result = result;
    }
}

/**
 * print_history - Display calculation history
 */
void print_history(void) {
    if (history_count == 0) {
        printf("No history yet.\n");
        return;
    }

    printf("\n═══════════════════════════════════════════════════════════\n");
    printf("                    CALCULATION HISTORY                    \n");
    printf("═══════════════════════════════════════════════════════════\n");
    for (int i = 0; i < history_count; i++) {
        printf(" %2d. %.2f %c %.2f = %.2f\n",
               i + 1,
               history[i].num1,
               history[i].op,
               history[i].num2,
               history[i].result);
    }
    printf("═══════════════════════════════════════════════════════════\n\n");
}

/**
 * perform_calculation - Execute the calculation
 */
double perform_calculation(double num1, char op, double num2, int* error) {
    double result = 0;
    *error = 0;

    switch (op) {
        case '+':
            result = num1 + num2;
            break;
        case '-':
            result = num1 - num2;
            break;
        case '*':
        case 'x':
        case 'X':
            result = num1 * num2;
            break;
        case '/':
            if (num2 == 0) {
                *error = 1;
                printf("Error: Division by zero!\n");
                return 0;
            }
            result = num1 / num2;
            break;
        case '%':
            if (num2 == 0) {
                *error = 1;
                printf("Error: Modulo by zero!\n");
                return 0;
            }
            result = fmod(num1, num2);
            break;
        case '^':
            result = pow(num1, num2);
            break;
        default:
            *error = 1;
            printf("Error: Invalid operator '%c'\n", op);
            return 0;
    }

    return result;
}

/**
 * print_help - Display help information
 */
void print_help(void) {
    printf("\n");
    printf("Available Operations:\n");
    printf("  +  Addition         (e.g., 5 + 3)\n");
    printf("  -  Subtraction      (e.g., 10 - 4)\n");
    printf("  *  Multiplication   (e.g., 6 * 7)\n");
    printf("  /  Division         (e.g., 20 / 4)\n");
    printf("  %%  Modulo          (e.g., 10 %% 3)\n");
    printf("  ^  Power            (e.g., 2 ^ 8)\n");
    printf("\n");
    printf("Special Commands:\n");
    printf("  history - Show calculation history\n");
    printf("  clear   - Clear history\n");
    printf("  help    - Show this help\n");
    printf("  exit    - Exit calculator\n");
    printf("\n");
}

/**
 * main - Calculator entry point
 *
 * Full-featured calculator with arithmetic operations,
 * history tracking, and continuous operation mode.
 *
 * Returns: 0 on success
 */
int main(void) {
    char input[256];
    double num1, num2, result;
    char op;

    printf("\n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("                  SCIENTIFIC CALCULATOR                     \n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("  PID: %d\n", getpid());
    printf("  Status: Running as REAL separate process\n");
    printf("════════════════════════════════════════════════════════════\n");

    print_help();

    printf("Ready for calculations!\n\n");

    while (1) {
        printf("calc> ");
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;  // EOF or error
        }

        // Remove newline
        input[strcspn(input, "\n")] = 0;

        // Check for empty input
        if (strlen(input) == 0) {
            continue;
        }

        // Check for special commands
        if (strcmp(input, "exit") == 0 || strcmp(input, "quit") == 0) {
            printf("\nThank you for using Calculator!\n");
            break;
        }

        if (strcmp(input, "history") == 0) {
            print_history();
            continue;
        }

        if (strcmp(input, "clear") == 0) {
            history_count = 0;
            printf("History cleared.\n");
            continue;
        }

        if (strcmp(input, "help") == 0) {
            print_help();
            continue;
        }

        // Try to parse calculation
        int parsed = sscanf(input, "%lf %c %lf", &num1, &op, &num2);

        if (parsed != 3) {
            printf("Invalid input format. Use: number operator number\n");
            printf("Example: 5 + 3\n");
            printf("Type 'help' for more information.\n");
            continue;
        }

        // Perform calculation
        int error = 0;
        result = perform_calculation(num1, op, num2, &error);

        if (!error) {
            printf("Result: %.2f %c %.2f = %.4f\n", num1, op, num2, result);
            add_to_history(num1, op, num2, result);
        }
    }

    printf("\nCalculator shutting down...\n");
    return 0;
}
