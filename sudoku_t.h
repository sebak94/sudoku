#ifndef __SUDOKU_T_H__
#define __SUDOKU_T_H__

#include "sudoku_cell_t.h"
#include <stdio.h>

// TDA que representa el tablero del Sudoku

#define SQUARE_SIZE 9

// En sudoku_t se guarda una matriz de SQUARE_SIZE * SQUARE_SIZE
// donde cada celda es de tipo sudoku_cell_t.
typedef struct {
    sudoku_cell_t board[SQUARE_SIZE][SQUARE_SIZE];
} sudoku_t;

// Carga el tablero inicial desde el archivo
int sudoku_init(sudoku_t *self);

// Agrega un valor a una celda del tablero
int sudoku_put(
    sudoku_t *self,
    char value,
    const size_t row,
    const size_t column
);

// Vuelve el tablero al estado inicial
int sudoku_reset(sudoku_t *self);

// Devuelve el estado actual del tablero de forma gráfica
int sudoku_get(sudoku_t *self, char *board);

// Verifica si todas las reglas se están cumpliendo
int sudoku_verify(sudoku_t *self);

// Libera todos los recursos tomados
void sudoku_release(sudoku_t *self);

#endif
