#include <tetris.h>
#include <display.h>
#include <board.h>

extern int delay;

extern int next_type;
extern Tetromino player;
extern Board board;

int rotate_cw(int c){
    int old[4][4];
    for (int i = 0; i < 4; i ++){
        for (int j = 0; j < 4; j ++){
            old[i][j] = player.map[i][j];
        }
    }

    if (player.type == I_PIECE-1 || player.type == O_PIECE-1){
        // rotation about centre of 4x4 map
        for (int y = 0; y < 4; y ++){
            for (int x = 0; x < 4; x ++){
                player.map[y][x] = old[3-x][y];
            }
        }
    }else{
        // rotation about centre of 3x3 inner map
        for (int y = 1; y < 4; y ++){
            for (int x = 0; x < 3; x ++){
                player.map[y][x] = old[3-x][y-1];
            }
        }
    }

    if (!no_collision_check()){
        if (c < 4){
            return rotate_cw(c+1);
        }
        return 0;
    }
    return 1;
}

int move_left(){
    player.x -= 1;
    if (!no_collision_check()){
        player.x += 1;
        return 0;
    }
    return 1;
}

int move_right(){
    player.x += 1;
    if (!no_collision_check()){
        player.x -= 1;
        return 0;
    }
    return 1;
}

int move_down(){
    player.y -= 1;
    if (!no_collision_check()){
        player.y += 1;
        save_piece();
        clear_lines();
        if (!next_piece()){
            player.y += 2;
            next_type = player.type;
            end_game();
        }
        return 0;
    }
    return 1;
}

// down up left right
int move_tetromino(int input){
    switch (input) {
        case KEY_DOWN:
        case 's':
            move_down();
            break;
        case KEY_LEFT:
        case 'a':
            move_left();
            break;
        case KEY_RIGHT:
        case 'd':
            move_right();
            break;
        case KEY_UP:
        case 'w':
            rotate_cw(1);
            break;
        case ' ':
            while(move_down()){}
            break;
    }
}