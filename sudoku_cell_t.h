#ifndef __SUDOKU_CELL_T_H__
#define __SUDOKU_CELL_T_H__

#include <stdbool.h>

// TDA que representa una celda del Sudoku

// En sudoku_cell_t se guarda el valor de la celda y si viene precargada
typedef struct {
    char value;
    bool is_initial;
} sudoku_cell_t;

#endif
