# Sudoku
This is a Client-Server Sudoku game made in C.

## Compile
>`make`

## Run
>`tp server 3000`

>`tp client localhost 3000`

## Commands
- **Get**: Return current board state representation.
>`get`

- **Verify**: Verify if the current state is valid.
>`verify`

- **Put**: Put a value in a cell.
>`put 2 in 8,7`
2: value
8: row
7: column

- **Exit**: Exit program.
>`exit`
