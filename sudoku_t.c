#include "sudoku_t.h"
#include "sudoku_cell_t.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define SUCCESS 0
#define ERROR 1
#define SQUARE_SIZE 9
#define SECTOR_SIZE 3
#define BOARD_REPRESENTATION_ROW_SIZE 38
#define NUMBERS "0123456789"
#define POSSIBLE_VALUES 10
#define BORDER "U===========U===========U===========U\n"
#define SEPARATOR "U---+---+---U---+---+---U---+---+---U\n"

int sudoku_init(sudoku_t *self) {
    FILE *fp;
    int character;
    int row = 0, column = 0;
    const char *boardFileName = "board.txt";

    if ((fp = fopen(boardFileName, "r")) == NULL) {
        fprintf(stderr, "Error leyendo %s.\n", boardFileName);
        return ERROR;
    }

    while ((character = fgetc(fp)) != EOF) {
        if (character == '\n') {
            row++;
            column = 0;
        }
        if (strchr(NUMBERS, character) != NULL) {
            sudoku_cell_t cell;
            cell.value = character == '0' ? ' ' : character;
            cell.is_initial = character == '0' ? false : true;
            self->board[row][column] = cell;
            column++;
        }
    }

    fclose(fp);
    return SUCCESS;
}

int sudoku_put(
    sudoku_t *self,
    char value,
    const size_t row,
    const size_t column
) {
    if (self->board[row - 1][column - 1].is_initial) return ERROR;
    
    self->board[row - 1][column - 1].value = value;

    return SUCCESS;
}

int sudoku_reset(sudoku_t *self) {
    for (size_t row = 0; row < SQUARE_SIZE; row++) {
        for (size_t column = 0; column < SQUARE_SIZE; column++) {
            if (!self->board[row][column].is_initial) {
                self->board[row][column].value = ' ';
            }
        }
    }

    return SUCCESS;
}

static char get_cell_value(
    const sudoku_t* self,
    const size_t row,
    const size_t column
) {
    return self->board[row][column].value;
}

static bool number_repeated(int number_flags[POSSIBLE_VALUES]) {
    bool number_repeated = false;
    size_t index = 1;

    while (!number_repeated && index < POSSIBLE_VALUES) {
        if (number_flags[index] > 1) number_repeated = true;
        index++;
    }

    return number_repeated;
}

static bool row_is_correct(sudoku_t *self, size_t row) {
    int number_flags[POSSIBLE_VALUES];
    memset(number_flags, 0, POSSIBLE_VALUES * sizeof(int));

    for (size_t column = 0; column < SQUARE_SIZE; column++) {
        char cvalue = get_cell_value(self, row, column);
        size_t ivalue = cvalue == ' ' ? 0 : (cvalue - '0');
        number_flags[ivalue]++;
    }

    return !number_repeated(number_flags);
}

static bool column_is_correct(sudoku_t *self, size_t column) {
    int number_flags[POSSIBLE_VALUES];
    memset(number_flags, 0, POSSIBLE_VALUES * sizeof(int));

    for (size_t row = 0; row < SQUARE_SIZE; row++) {
        char cvalue = get_cell_value(self, row, column);
        size_t ivalue = cvalue == ' ' ? 0 : (cvalue - '0');
        number_flags[ivalue]++;
    }

    return !number_repeated(number_flags);
}

static bool sector_is_correct(
    sudoku_t *self,
    size_t start_row,
    size_t start_column
) {
    int number_flags[POSSIBLE_VALUES];
    memset(number_flags, 0, POSSIBLE_VALUES * sizeof(int));

    for (size_t row = 0 + start_row; row < SECTOR_SIZE; row++) {
        for (size_t column = 0 + start_column; column < SECTOR_SIZE; column++) {
            char cvalue = get_cell_value(self, row, column);
            int ivalue = cvalue == ' ' ? 0 : (cvalue - '0');
            number_flags[ivalue]++;
        }
    }

    return !number_repeated(number_flags);
}

int sudoku_verify(sudoku_t *self) {
    for (size_t i = 0; i < SQUARE_SIZE; i++) {
        if (!row_is_correct(self, i) || !column_is_correct(self, i)) {
            return ERROR;
        }
    }

    for (size_t i = 0; i < SECTOR_SIZE; i++) {
        for (size_t j = 0; j < SECTOR_SIZE; j++) {
            if (!sector_is_correct(self, SECTOR_SIZE * j, SECTOR_SIZE * i)) {
                return ERROR;
            }
        }
    }

    return SUCCESS;
}

int sudoku_get(sudoku_t *self, char *board) {
    char *board_ptr = &board[0];
    snprintf(board, BOARD_REPRESENTATION_ROW_SIZE + 1, BORDER);
    board += BOARD_REPRESENTATION_ROW_SIZE;
    for (size_t row = 0; row < SQUARE_SIZE; row++) {
        snprintf(board++, sizeof(board + 1), "U");
        for (size_t column = 0; column < SQUARE_SIZE; column++) {
            snprintf(board, sizeof(board + 4), " %c ",
                get_cell_value(self, row, column));
            board += 3;
            snprintf(board++, sizeof(board + 1),
                (column > 0 && (column + 1) % 3 == 0) ? "U" : "|");
        }
        snprintf(board++, sizeof(board + 1), "\n");
        snprintf(board, BOARD_REPRESENTATION_ROW_SIZE + 1,
            (row + 1) % 3 == 0 ? BORDER : SEPARATOR);
        board += BOARD_REPRESENTATION_ROW_SIZE;
    }
    board = board_ptr;

    return SUCCESS;
}

void sudoku_release(sudoku_t *self) {
    // do nothing
}
