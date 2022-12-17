#ifndef TETRIS
#define TETRIS

#include <ncurses.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define I_PIECE 1
#define L_PIECE 2
#define J_PIECE 3
#define T_PIECE 4
#define O_PIECE 5
#define S_PIECE 6
#define Z_PIECE 7

#define BOARD_X 11
#define BOARD_Y 7

typedef struct piece {
    int map[4][4];
    int type;
    int x;
    int y;
} Tetromino;

typedef struct pieces {
    int len;
    Tetromino data[200];
} Board;

#endif