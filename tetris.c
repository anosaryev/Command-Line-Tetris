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

#define DEBUG 0


int top;
int status = 0;
int score = 0;
int level = 0;
int lines = 0;

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
        {1, 0, 0, 0},
        {1, 1, 1, 0},
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
int printing = 0;

int delay = 999999999;
int bag_idx = 7;
int bag[7] = {0, 1, 2, 3, 4, 5, 6};

int choose_piece(){
    if (bag_idx == 7){
        bag_idx = 0;
        for (int i = 0; i < 7; i++){
            int swap = rand() % 7;
            int temp = bag[i];
            bag[i] = bag[swap];
            bag[swap] = temp;
        }
    }else{
        bag_idx += 1;
    }

    return bag[bag_idx];
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

int get_top(){
    int n = 0;

    FILE *f;
    char str[10] = "";
    f = fopen("user.dat", "r");
    char c = fgetc(f);
    while (c != '\n' && c != EOF) {
        n = n*10 + atoi(&c);
        c = fgetc(f);
    }

    return n;
}

int set_top(){
    if (top > get_top()){
        FILE *f;
        char str[10] = "";
        sprintf(str, "%d", top);
        f = fopen("user.dat", "w");
        fputs(str, f);
        fputs("\n", f);
    }

    return 1;
}

int update_stats(int tiles){
    lines += tiles/10;
    switch (tiles/10){
        case 1:
            score += 40 * (level + 1);
            break;
        case 2:
            score += 100 * (level + 1);
            break;
        case 3:
            score += 300 * (level + 1);
            break;
        case 4:
            score += 1200 * (level + 1);
            break;
    }
    if (score > top){
        top = score;
        set_top();
    }

    if (lines/10 > level){
        level++;
        delay *= 0.8;
    }
    return 1;
}

int init_colors(){
    start_color();
    init_pair(I_PIECE, COLOR_CYAN, COLOR_CYAN);
    init_pair(L_PIECE, COLOR_YELLOW, COLOR_YELLOW);
    init_pair(J_PIECE, COLOR_BLUE, COLOR_BLUE);
    init_pair(T_PIECE, COLOR_MAGENTA, COLOR_MAGENTA);
    init_pair(O_PIECE, COLOR_WHITE, COLOR_WHITE);
    init_pair(S_PIECE, COLOR_GREEN, COLOR_GREEN);
    init_pair(Z_PIECE, COLOR_RED, COLOR_RED);
    return 1;
}

void print_tile(int y, int x, int type){
    switch (type){
        case 0:
            mvprintw(y, x, ". ");
            break;
        case (I_PIECE):
            attron(COLOR_PAIR(I_PIECE));
            mvprintw(y, x, "  ");
            attroff(COLOR_PAIR(I_PIECE));
            break;
        case (L_PIECE):
            attron(COLOR_PAIR(L_PIECE));
            mvprintw(y, x, "  ");
            attroff(COLOR_PAIR(L_PIECE));
            break;
        case (J_PIECE):
            attron(COLOR_PAIR(J_PIECE));
            mvprintw(y, x, "  ");
            attroff(COLOR_PAIR(J_PIECE));
            break;
        case (T_PIECE):
            attron(COLOR_PAIR(T_PIECE));
            mvprintw(y, x, "  ");
            attroff(COLOR_PAIR(T_PIECE));
            break;
        case (O_PIECE):
            attron(COLOR_PAIR(O_PIECE));
            mvprintw(y, x, "  ");
            attroff(COLOR_PAIR(O_PIECE));
            break;
        case (S_PIECE):
            attron(COLOR_PAIR(S_PIECE));
            mvprintw(y, x, "  ");
            attroff(COLOR_PAIR(S_PIECE));
            break;
        case (Z_PIECE):
            attron(COLOR_PAIR(Z_PIECE));
            mvprintw(y, x, "  ");
            attroff(COLOR_PAIR(Z_PIECE));
            break;
        default:
            mvprintw(y, x, ". ");
            break;
    }
}

int print_stats(){
    char str[10] = "";
    int x, y;

    y = -1;
    sprintf(str, "%d", top);
    mvprintw(4+y, 4, "Top:    %-9.9s", str);

    y++;
    sprintf(str, "%d", score);
    mvprintw(4+y, 4, "Score:  %-9.9s   ", str);

    y ++;
    sprintf(str, "%d", level);
    mvprintw(4+y, 4, "Level:  %-9.9s   ", str);

    y ++;
    sprintf(str, "%d", lines);
    mvprintw(4+y, 4, "Lines:  %-9.9s   ", str);
    return 1;
}

int print_next(){
    mvprintw(3, 26, "Next");
    for (int y = 0; y < 3; y ++){
        for (int x = 0; x < 4; x ++){
            if (maps[next_type][y][x] == 1){
                print_tile(4+y, 24 + 2*x, next_type+1);
            }else{
                
                mvprintw(4+y, 24 + 2*x, "  ");
            }
        }
    }
    return 1;
}

int print_board(){
    erase();
    mvprintw(1, 9, "Command Line Tetris\n\n");

    print_stats();
    print_next();

    int x, y, i;
    for (y = 19; y >= 0; y--){
        mvprintw(BOARD_Y+19 - y, 6, "%02d", y);

        for (x = 0; x < 10; x++){
            print_tile(BOARD_Y+19 - y, BOARD_X + 2*x, 0);
         }
    }
    mvprintw(BOARD_Y+21, 11, "0 1 2 3 4 5 6 7 8 9");
    return 1;
}

int print_controls(){
    int y = BOARD_Y + 4;
    int x = BOARD_X + 24;
    if (status){
        mvprintw(y++, x+7, "Controls\n");
        y++;
        mvprintw(y++, x+1, "Left or A: Move Left\n");
        mvprintw(y++, x+0, "Right or D: Move Right\n");
        mvprintw(y++, x+2, "Up or W: Rotate CW\n");
        y++;
        mvprintw(y++, x+1, "Down or S: Soft Drop\n");
        mvprintw(y++, x+3, "Space: Hard Drop\n");
        y++;
        mvprintw(y, x+5, "Q: Quit Game\n");
    }
    return 1;
}

int update_board(){
    int y, x, i;
    for (y = 19; y >= 0; y--){
        for (x = 0; x < 10; x++){

            // search player piece for something at coords
            if (x - player.x < 4 && x - player.x >= 0 &&
                player.y - y < 4 && player.y - y >= 0 &&
                player.map[player.y - y][x - player.x] == 1){
                print_tile(BOARD_Y+19 - y, BOARD_X + 2*x, player.type+1);
            }else{

                // search every board piece for something at coords
                for (i = 0; i < board.len; i++){
                    Tetromino temp = board.data[i];
                    if (x - temp.x < 4 && x - temp.x >= 0 &&
                        temp.y - y < 4 && temp.y - y >= 0){
                        if (temp.map[temp.y - y][x - temp.x] == 1){
                            // draw board piece with colour based on type
                            print_tile(BOARD_Y+19 - y, BOARD_X + 2*x, temp.type+1);
                            break;
                        }
                    }
                }
                
                // no player or board piece
                if (i == board.len){
                    print_tile(BOARD_Y+19 - y, BOARD_X + 2*x, 0);
                }
            }
        }
    }
    print_stats();
    return 1;
}

int clear_lines(){
    int counts[20] = {};
    int tiles = 0;

    for (int i = 0; i < board.len; i ++){
        counts[board.data[i].y] += 1;
    }

    for (int i = board.len-1; i >= 0; i --){
        if (counts[board.data[i].y] == 10){
            tiles += 1;
            for (int j = i; j < board.len; j ++){
                board.data[j] = board.data[j+1];
            }
            board.len --;
        }else{
            int counter = 0;
            for (int j = 0; j < board.data[i].y; j++){
                if (counts[j] == 10){
                    counter ++;
                }
            }
            board.data[i].y -= counter;
        }
    }

    return update_stats(tiles);
}

int next_piece(){
    player.type = next_type;
    player.x = 3;
    player.y = 20;
    memcpy(player.map, maps[player.type], sizeof(int[4][4]));

    next_type = choose_piece();
    print_next();

    if (!no_collision_check()){
        return 0;
    }

    return 1;
}

int end_game(){
    int y = BOARD_Y + 8;
    int x = BOARD_X + 24;

    status = 0;
    print_board();
    mvprintw(y++, x+8, "Game over!\n");
    mvprintw(y++, x+2, "Press any key to quit.\n");
    getch();

    top = set_top();
    endwin();
    exit(0);
}

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
            return rotate_loop(c+1);
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

void *move_down_passive(){
    while (1){
        struct timespec t0 = {0, delay};
        nanosleep(&t0, NULL);
        move_down();
        update_board();
        refresh();
    }
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
        case 'q':
        case '/':
            if (DEBUG){
                player.y ++;
            }
            break;
    }
}

int main(){
    srand(time(0));
    
    initscr();
    cbreak();
    noecho();
    clear();
    init_colors();
    curs_set(0);
    keypad(stdscr, TRUE);
    
    top = get_top();
    next_type = choose_piece();
    next_piece();
    status = 1;

    int q;
    print_board();
    update_board();
    print_controls();
    q = getch();
 
    // moving down thread
    pthread_t down_id, render_id;
    pthread_create(&down_id, NULL, move_down_passive, NULL);
    
    while (q != 'q' && status){
        move_tetromino(q);
        update_board();
        q = getch();
    }
    
    top = set_top();
    endwin();
    exit(0);
}