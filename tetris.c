#include <ncurses.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define I_PIECE 0
#define L_PIECE 1
#define J_PIECE 2
#define T_PIECE 3
#define O_PIECE 4
#define S_PIECE 5
#define Z_PIECE 6

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

    if (lines/10 > level){
        level++;
        Board new_board = {0, {}};
        board = new_board;
    }
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

    next_type = choose_piece(&bag_idx);

    if (!no_collision_check()){
        return 0;
    }

    return 1;
}

void print_tile(int type){
    start_color();
    switch (type){
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
            printw(". ");
            break;
    }
}

int print_stats_next(){
    char str[10] = "";
    int x, y;

    sprintf(str, "%d", 0);
    printw("    Top:    %-9.9s", str);
    printw("     Next");

    y = 0;
    sprintf(str, "%d", score);
    printw("\n    Score:  %-9.9s   ", str);
    for (x = 0; x < 4; x ++){
        (maps[next_type][y][x] == 1)? print_tile(next_type) : printw("  ");
    }

    y ++;
    sprintf(str, "%d", level);
    printw("\n    Level:  %-9.9s   ", str);
    for (x = 0; x < 4; x ++){
        (maps[next_type][y][x] == 1)? print_tile(next_type) : printw("  ");
    }

    y ++;
    sprintf(str, "%d", lines);
    printw("\n    Lines:  %-9.9s   ", str);
    for (x = 0; x < 4; x ++){
        (maps[next_type][y][x] == 1)? print_tile(next_type) : printw("  ");
    }
    printw("\n\n");
    return 1;
}

int print_game(){
    erase();
    printw("         Command Line Tetris\n\n");

    print_stats_next();
    // displays temporary placeholder values
    
    int x, y, i;
    for (y = 19; y >= 0; y--){
        printw("      %02d   ", y);
        
        for (x = 0; x < 10; x++){
            // search player piece for something at coords
            if (x - player.x < 4 && x - player.x >= 0 &&
                player.y - y < 4 && player.y - y >= 0 &&
                player.map[player.y - y][x - player.x] == 1){
                    print_tile(player.type);
            }else{

                // search every board piece for something at coords
                for (i = 0; i < board.len; i++){
                    Tetromino temp = board.data[i];
                    if (x - temp.x < 4 && x - temp.x >= 0 &&
                        temp.y - y < 4 && temp.y - y >= 0){
                        if (temp.map[temp.y - y][x - temp.x] == 1){
                            // draw board piece with colour based on type
                            print_tile(temp.type);
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
    printw("\n           0 1 2 3 4 5 6 7 8 9\n\n");

    if (status){
        printw("              Controls:\n");
        printw("         L or A: Move Left\n");
        printw("         \" or D: Move Right\n");
        printw("         P or W: Rotate CW\n");
        printw("         ; or S: Soft Drop\n");
        printw("          Space: Hard Drop\n");
        printw("            Q: Quit Game");
    }
}

int end_game(){
    status = 0;
    print_game();
    printw("\n              Game over!\n");
    printw("        Press any key to quit.\n");
    getch();
    endwin();
    exit(0);
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
        struct timespec t0 = {1, 0};
        nanosleep(&t0, NULL);
        move_down();
        print_game();
        refresh();
    }
}

int move_tetromino(char input){
    switch (input) {
        case ';':
        case 's':
            move_down();
            break;
        case 'l':
        case 'a':
            move_left();
            break;
        case '\'':
        case 'd':
            move_right();
            break;
        case 'p':
        case 'w':
            rotate_cw();
            break;
        case ' ':
            while(move_down()){}
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
    status = 1;

    char q;
    print_game();
    refresh();
    q = getch();

    // moving down thread
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, move_down_passive, NULL);
    
    while (q != 'q' && status){
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

    Top:    0000000     Next
    Score:  0000000  [  next  ]
    Level:  0000000  [  piece ]
    Lines:  0000000  [  here  ]

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
           
              Controls:
         L or A: Move Left
         " or D: Move Right
         P or W: Rotate CW
         ; or S: Soft Drop
          Space: Hard Drop
            Q: Quit Game
*/       