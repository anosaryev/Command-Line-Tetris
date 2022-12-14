#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <pthread.h>

#define I_PIECE 0
#define L_PIECE 1
#define J_PIECE 2
#define T_PIECE 3
#define O_PIECE 4
#define S_PIECE 5
#define Z_PIECE 6

int maps[7][4][4] = 
{
    { /* I */
        {0, 0, 0, 0},
        {1, 1, 1, 1},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    }, { /* L */
        {0, 0, 0, 0},
        {0, 0, 1, 0},
        {1, 1, 1, 0},
        {0, 0, 0, 0} 
    }, { /* J */
        {0, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 1, 1, 1},
        {0, 0, 0, 0}
    }, { /* T */
        {0, 0, 0, 0},
        {0, 1, 0, 0},
        {1, 1, 1, 0},
        {0, 0, 0, 0}
    }, { /* O */
        {0, 0, 0, 0},
        {0, 1, 1, 0},
        {0, 1, 1, 0},
        {0, 0, 0, 0}
    }, { /* S */
        {0, 0, 0, 0},
        {0, 1, 1, 0},
        {1, 1, 0, 0},
        {0, 0, 0, 0}
    }, { /* Z */
        {0, 0, 0, 0},
        {1, 1, 0, 0},
        {0, 1, 1, 0},
        {0, 0, 0, 0}
    }
};

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

int next_type;
Tetromino player;
Board board = {0, {}};

int bag_idx = 7;
int bag[7] = {0, 1, 2, 3, 4, 5, 6};

int choose_piece(int *bag_idx){
    if (*bag_idx == 7){
        *bag_idx = 0;
        for (int i = 0; i < 7; i++){
            int swap = rand() % 7;
            int temp = bag[i];
            bag[i] = bag[swap];
            bag[swap] = temp;
        }
    }else{
        *bag_idx += 1;
    }

    return bag[*bag_idx];
}

int save_piece(){

    // splits player tetromino into 4 1x1 tetrominos
    int temp_idx = 0;
    Tetromino temp[4] = {};
    for (int y = 0; y < 4; y ++){
        for (int x = 0; x < 4; x ++){
            if (player.map[y][x] == 1){
                temp[temp_idx].type = player.type;
                temp[temp_idx].x = player.x + x;
                temp[temp_idx].y = player.y - y;
                temp_idx ++;
            }

            if (x == 0 && y == 0){
                temp[0].map[y][x] = 1;
                temp[1].map[y][x] = 1;
                temp[2].map[y][x] = 1;
                temp[3].map[y][x] = 1;
            }else{
                temp[0].map[y][x] = 0;
                temp[1].map[y][x] = 0;
                temp[2].map[y][x] = 0;
                temp[3].map[y][x] = 0;
            }
        }
    }

    for (int i = 0; i < 4; i ++){
        board.data[board.len++] = temp[i];
    }
}

int next_piece(){
    player.type = next_type;
    player.x = 3;
    player.y = 20;
    memcpy(player.map, maps[player.type], sizeof(int[4][4]));

    next_type = choose_piece(&bag_idx);

    return 1;
}

void print_tile(Tetromino curr){
    start_color();
    switch (curr.type){
        case (I_PIECE):
            init_pair(I_PIECE+1, COLOR_CYAN, COLOR_CYAN);
            attron(COLOR_PAIR(I_PIECE+1));
            printw("  ");
            attroff(COLOR_PAIR(I_PIECE+1));
            break;
        case (L_PIECE):
            init_pair(L_PIECE+1, COLOR_YELLOW, COLOR_YELLOW);
            attron(COLOR_PAIR(L_PIECE+1));
            printw("  ");
            attroff(COLOR_PAIR(L_PIECE+1));
            break;
        case (J_PIECE):
            init_pair(J_PIECE+1, COLOR_BLUE, COLOR_BLUE);
            attron(COLOR_PAIR(J_PIECE+1));
            printw("  ");
            attroff(COLOR_PAIR(J_PIECE+1));
            break;
        case (T_PIECE):
            init_pair(T_PIECE+1, COLOR_MAGENTA, COLOR_MAGENTA);
            attron(COLOR_PAIR(T_PIECE+1));
            printw("  ");
            attroff(COLOR_PAIR(T_PIECE+1));
            break;
        case (O_PIECE):
            init_pair(O_PIECE+1, COLOR_WHITE, COLOR_WHITE);
            attron(COLOR_PAIR(O_PIECE+1));
            printw("  ");
            attroff(COLOR_PAIR(O_PIECE+1));
            break;
        case (S_PIECE):
            init_pair(S_PIECE+1, COLOR_GREEN, COLOR_GREEN);
            attron(COLOR_PAIR(S_PIECE+1));
            printw("  ");
            attroff(COLOR_PAIR(S_PIECE+1));
            break;
        case (Z_PIECE):
            init_pair(Z_PIECE+1, COLOR_RED, COLOR_RED);
            attron(COLOR_PAIR(Z_PIECE+1));
            printw("  ");
            attroff(COLOR_PAIR(Z_PIECE+1));
            break;
        default:
            printw("bagel falafel eggs and mac and cheese");
            break;
    }
}

int print_game(){
    clear();
    printw("         Command Line Tetris\n\n");
    printw("    Top:    %d\n", 0); // and next Tetromino component
    printw("    Score:  %d\n", 0); // and next Tetromino component
    printw("    Level:  %d\n", 0); // and next Tetromino component
    printw("    Lines:  %d\n", 0); // and next Tetromino component
    printw("\n");
    
    int x, y, i;
    for (y = 19; y >= 0; y--){
        printw("      %02d   ", y);
        
        for (x = 0; x < 10; x++){
            // search player piece for something at coords
            if (x - player.x < 4 && x - player.x >= 0 &&
                player.y - y < 4 && player.y - y >= 0 &&
                player.map[player.y - y][x - player.x] == 1){
                    print_tile(player);
            }else{

                // search every board piece for something at coords
                for (i = 0; i < board.len; i++){
                    Tetromino temp = board.data[i];
                    if (x - temp.x < 4 && x - temp.x >= 0 &&
                        temp.y - y < 4 && temp.y - y >= 0){
                        if (temp.map[temp.y - y][x - temp.x] == 1){
                            // draw board piece with colour based on type
                            print_tile(temp);
                            break;
                        }
                    }
                }
                
                // no player or board piece
                if (i == board.len){
                    printw(". ");
                }
            }
        }
        printw("\n");
    }
    printw("\n           0 1 2 3 4 5 6 7 8 9\n");
}

int no_collision_check(){
    for (int y = 0; y < 4; y ++){
        for (int x = 0; x < 4; x ++){
            if (player.map[y][x] == 1){
                
                // collision with wall
                if (player.x + x < 0 || player.x + x > 9 ||
                    player.y - y > 19 || player.y - y < 0){
                    return 0;
                }

                // collision with other tetrominoes
                for (int i = 0; i < board.len; i ++){
                    if (board.data[i].x == player.x + x &&
                        board.data[i].y == player.y - y){
                        return 0;
                    }
                }
            }
        }
    }
    return 1;
}

int rotate_cw(){
    int new[4][4];

    for (int y = 0; y < 4; y ++){
        for (int x = 0; x < 4; x ++){
            new[y][x] = player.map[3-x][y];
        }
    }

    for (int i = 0; i < 4; i ++){
        for (int j = 0; j < 4; j ++){
            player.map[i][j] = new[i][j];
        }
    }

    if (!no_collision_check()){
        for (int y = 0; y < 4; y ++){
            for (int x = 0; x < 4; x ++){
                new[y][x] = player.map[x][3-y];
            }
        }

        for (int i = 0; i < 4; i ++){
            for (int j = 0; j < 4; j ++){
                player.map[i][j] = new[i][j];
            }
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
        next_piece();
        return 0;
    }
    return 1;
}

void *move_down_passive(){
    while (1){
        struct timespec t0 = {1, 0};
        nanosleep(&t0, NULL);
        if (move_down() == 0){
            // do something
        }
        print_game();
        refresh();
    }
}

int move_tetromino(char input){
    switch (input) {
        case 'l':
            move_left();
            break;
        case '\'':
            move_right();
            break;
        case ';':
            move_down();
            break;
        case 'p':
            rotate_cw();
            break;
        case 'w': // DEBUG PURPOSES
            player.y += 1;
            break;
    }
}

int main(){
    srand(time(0));

    initscr();
    cbreak();
    noecho();
    clear();
    
    next_type = choose_piece(&bag_idx);
    next_piece();

    char q;
    print_game();
    refresh();
    q = getch();

    // moving down thread
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, move_down_passive, NULL);

    while (q != 'q'){
        move_tetromino(q);
        print_game();
        refresh();
        q = getch();
    }
    
    endwin();
    exit(0);
    return 1;
}

/*
         Command Line Tetris

    Top:    w           [````````]
    Score:  x           [  next  ]
    Level:  y           Tetromino]
    Lines:  z           [________]

      19   . . . . . . . . . .
      18   . . . . . . . . . .
      17   . . . . . . . . . .
      16   . . . . . . . . . .
      15   . . . . . . . . . .
      14   . . . . . . . . . .
      13   . . . . . . . . . .
      12   . . . . . . . . . .
      11   . . . . . . . . . .
      10   . . . . . . . . . .
      09   . . . . . . . . . .
      08   . . . . . . . . . .
      07   . . . . . . . . . .
      06   . . . . . . . . . .
      05   . . . . . . . . . .
      04   . . . . . . . . . .
      03   . . . . . . . . . .
      02   . . . . . . . . . .
      01   . . . . . . . . . .
      00   . . . . . . . . . .

           0 1 2 3 4 5 6 7 8 9
*/       