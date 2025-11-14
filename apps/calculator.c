/**
 * calculator.c - Scientific Calculator Application
 * Calculator with operation history
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

/**
 * main - Calculator entry point
 *
 * Provides basic arithmetic operations with history.
 *
 * Returns: 0 on success
 */
int main(void) {
    char input[100];
    double num1, num2, result;
    char op;

    printf("\n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("                  SCIENTIFIC CALCULATOR                     \n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("  PID: %d\n", getpid());
    printf("  Status: Running as separate process\n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("\n");

    printf("Operations: +, -, *, /\n");
    printf("Format: number1 operator number2 (e.g., 5 + 3)\n");
    printf("Type 'exit' to quit\n");
    printf("\n");

    printf("History:\n");
    printf("─────────────────────────────────────────────────────────────\n");

    while (1) {
        printf("\nEnter calculation: ");
        fgets(input, sizeof(input), stdin);

        if (strncmp(input, "exit", 4) == 0) {
            break;
        }

        if (sscanf(input, "%lf %c %lf", &num1, &op, &num2) == 3) {
            switch (op) {
                case '+':
                    result = num1 + num2;
                    break;
                case '-':
                    result = num1 - num2;
                    break;
                case '*':
                    result = num1 * num2;
                    break;
                case '/':
                    if (num2 != 0) {
                        result = num1 / num2;
                    } else {
                        printf("Error: Division by zero\n");
                        continue;
                    }
                    break;
                default:
                    printf("Invalid operator\n");
                    continue;
            }

            printf("Result: %.2f %c %.2f = %.2f\n", num1, op, num2, result);
        } else {
            printf("Invalid input format\n");
        }
    }

    printf("Calculator closing...\n");
    return 0;
}
