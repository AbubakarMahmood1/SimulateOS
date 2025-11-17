/**
 * minesweeper.c - Minesweeper Game
 * Classic minesweeper game with flood fill, flags, and win/loss detection
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>

#define SIZE 10
#define MINES 15
#define MINE -1

// Cell states
#define HIDDEN 0
#define REVEALED 1
#define FLAGGED 2

typedef struct {
    int value;      // -1 for mine, 0-8 for number
    int state;      // HIDDEN, REVEALED, FLAGGED
} Cell;

Cell board[SIZE][SIZE];
int game_over = 0;
int won = 0;
int flags_placed = 0;
int cells_revealed = 0;
int total_safe_cells = 0;

/**
 * init_board - Initialize empty board
 */
void init_board(void) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            board[i][j].value = 0;
            board[i][j].state = HIDDEN;
        }
    }
    cells_revealed = 0;
    flags_placed = 0;
    total_safe_cells = SIZE * SIZE - MINES;
}

/**
 * place_mines - Randomly place mines on board
 */
void place_mines(void) {
    int placed = 0;

    while (placed < MINES) {
        int row = rand() % SIZE;
        int col = rand() % SIZE;

        if (board[row][col].value != MINE) {
            board[row][col].value = MINE;
            placed++;
        }
    }
}

/**
 * count_adjacent_mines - Count mines around a cell
 */
int count_adjacent_mines(int row, int col) {
    int count = 0;

    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i == 0 && j == 0) continue;

            int new_row = row + i;
            int new_col = col + j;

            if (new_row >= 0 && new_row < SIZE &&
                new_col >= 0 && new_col < SIZE) {
                if (board[new_row][new_col].value == MINE) {
                    count++;
                }
            }
        }
    }

    return count;
}

/**
 * calculate_numbers - Calculate numbers for all non-mine cells
 */
void calculate_numbers(void) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (board[i][j].value != MINE) {
                board[i][j].value = count_adjacent_mines(i, j);
            }
        }
    }
}

/**
 * reveal_cell - Recursively reveal cells (flood fill)
 */
void reveal_cell(int row, int col) {
    // Bounds check
    if (row < 0 || row >= SIZE || col < 0 || col >= SIZE) {
        return;
    }

    // Already revealed or flagged
    if (board[row][col].state != HIDDEN) {
        return;
    }

    // Reveal this cell
    board[row][col].state = REVEALED;
    cells_revealed++;

    // If it's a mine, game over
    if (board[row][col].value == MINE) {
        game_over = 1;
        won = 0;
        return;
    }

    // If it's a number, stop here
    if (board[row][col].value > 0) {
        return;
    }

    // If it's 0, reveal all adjacent cells (flood fill)
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i == 0 && j == 0) continue;
            reveal_cell(row + i, col + j);
        }
    }
}

/**
 * toggle_flag - Place or remove flag
 */
void toggle_flag(int row, int col) {
    if (board[row][col].state == HIDDEN) {
        board[row][col].state = FLAGGED;
        flags_placed++;
    } else if (board[row][col].state == FLAGGED) {
        board[row][col].state = HIDDEN;
        flags_placed--;
    }
}

/**
 * check_win - Check if player has won
 */
void check_win(void) {
    if (cells_revealed == total_safe_cells) {
        game_over = 1;
        won = 1;
    }
}

/**
 * print_board - Display the game board
 */
void print_board(int reveal_all) {
    printf("\n");
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║                     MINESWEEPER GAME                         ║\n");
    printf("╠══════════════════════════════════════════════════════════════╣\n");
    printf("║  Mines: %2d    Flags: %2d    Revealed: %3d / %3d            ║\n",
           MINES, flags_placed, cells_revealed, total_safe_cells);
    printf("╚══════════════════════════════════════════════════════════════╝\n");
    printf("\n");

    // Column numbers
    printf("     ");
    for (int j = 0; j < SIZE; j++) {
        printf("%2d ", j);
    }
    printf("\n");

    printf("    ");
    for (int j = 0; j < SIZE; j++) {
        printf("───");
    }
    printf("\n");

    // Board rows
    for (int i = 0; i < SIZE; i++) {
        printf(" %2d │", i);

        for (int j = 0; j < SIZE; j++) {
            if (reveal_all) {
                // Reveal everything (for game over)
                if (board[i][j].value == MINE) {
                    printf(" * ");
                } else if (board[i][j].value == 0) {
                    printf(" . ");
                } else {
                    printf(" %d ", board[i][j].value);
                }
            } else {
                // Normal display
                if (board[i][j].state == HIDDEN) {
                    printf(" █ ");
                } else if (board[i][j].state == FLAGGED) {
                    printf(" ⚑ ");
                } else {  // REVEALED
                    if (board[i][j].value == MINE) {
                        printf(" ☀ ");  // Exploded mine
                    } else if (board[i][j].value == 0) {
                        printf(" . ");
                    } else {
                        printf(" %d ", board[i][j].value);
                    }
                }
            }
        }

        printf("\n");
    }

    printf("\n");
}

/**
 * print_help - Display help information
 */
void print_help(void) {
    printf("\n");
    printf("═══════════════════════════════════════════════════════════\n");
    printf("                       HOW TO PLAY                         \n");
    printf("═══════════════════════════════════════════════════════════\n");
    printf("Commands:\n");
    printf("  r <row> <col>  - Reveal cell at position\n");
    printf("  f <row> <col>  - Toggle flag at position\n");
    printf("  help           - Show this help\n");
    printf("  exit           - Quit game\n");
    printf("\n");
    printf("Symbols:\n");
    printf("  █  - Hidden cell\n");
    printf("  ⚑  - Flagged cell (suspected mine)\n");
    printf("  .  - Empty revealed cell\n");
    printf("  1-8 - Number of adjacent mines\n");
    printf("  ☀  - Exploded mine (game over!)\n");
    printf("  *  - Mine (shown after game ends)\n");
    printf("\n");
    printf("Goal: Reveal all safe cells without hitting a mine!\n");
    printf("═══════════════════════════════════════════════════════════\n");
    printf("\n");
}

/**
 * main - Minesweeper entry point
 *
 * Full minesweeper game with flood fill, flagging, and win detection.
 *
 * Returns: 0 on success
 */
int main(void) {
    char input[100];
    char command;
    int row, col;

    // Initialize random seed
    srand(time(NULL) ^ getpid());

    printf("\n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("                       MINESWEEPER                          \n");
    printf("════════════════════════════════════════════════════════════\n");
    printf("  PID: %d\n", getpid());
    printf("  Status: Running as REAL separate process\n");
    printf("  Features: Flood fill, flags, win/loss detection\n");
    printf("════════════════════════════════════════════════════════════\n");

    // Initialize game
    init_board();
    place_mines();
    calculate_numbers();

    printf("\n");
    printf("Welcome to Minesweeper!\n");
    printf("Board: %dx%d with %d mines\n", SIZE, SIZE, MINES);
    printf("Type 'help' for instructions\n");

    print_help();

    // Game loop
    while (!game_over) {
        print_board(0);

        printf("Command> ");
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }

        // Remove trailing newline
        input[strcspn(input, "\n")] = 0;

        // Skip empty input
        if (strlen(input) == 0) {
            continue;
        }

        // Check for exit
        if (strcmp(input, "exit") == 0 || strcmp(input, "quit") == 0) {
            printf("\nExiting game...\n");
            break;
        }

        // Check for help
        if (strcmp(input, "help") == 0) {
            print_help();
            continue;
        }

        // Parse command
        if (sscanf(input, " %c %d %d", &command, &row, &col) != 3) {
            printf("Invalid input. Use: r <row> <col> or f <row> <col>\n");
            printf("Type 'help' for instructions.\n");
            continue;
        }

        // Validate coordinates
        if (row < 0 || row >= SIZE || col < 0 || col >= SIZE) {
            printf("Invalid coordinates. Row and column must be 0-%d\n", SIZE - 1);
            continue;
        }

        // Process command
        command = tolower(command);

        if (command == 'r') {
            // Reveal cell
            if (board[row][col].state == FLAGGED) {
                printf("Cannot reveal flagged cell. Remove flag first.\n");
                continue;
            }

            if (board[row][col].state == REVEALED) {
                printf("Cell already revealed.\n");
                continue;
            }

            reveal_cell(row, col);

            if (board[row][col].value == MINE) {
                // Hit a mine
                printf("\n");
                printf("╔══════════════════════════════════════════════════════════════╗\n");
                printf("║                       ☀ BOOM! ☀                              ║\n");
                printf("║                    YOU HIT A MINE!                           ║\n");
                printf("║                      GAME OVER                               ║\n");
                printf("╚══════════════════════════════════════════════════════════════╝\n");
                game_over = 1;
                won = 0;
            } else {
                // Check if won
                check_win();
            }

        } else if (command == 'f') {
            // Toggle flag
            if (board[row][col].state == REVEALED) {
                printf("Cannot flag revealed cell.\n");
                continue;
            }

            toggle_flag(row, col);

        } else {
            printf("Unknown command '%c'. Use 'r' to reveal or 'f' to flag.\n", command);
        }
    }

    // Game over - show final board
    if (game_over) {
        print_board(1);

        if (won) {
            printf("\n");
            printf("╔══════════════════════════════════════════════════════════════╗\n");
            printf("║                    ★ CONGRATULATIONS! ★                      ║\n");
            printf("║                      YOU WON!                                ║\n");
            printf("║           All safe cells revealed successfully!             ║\n");
            printf("╚══════════════════════════════════════════════════════════════╝\n");
            printf("\n");
        } else {
            printf("\nBetter luck next time!\n\n");
        }
    }

    printf("Minesweeper closing...\n");
    return 0;
}
