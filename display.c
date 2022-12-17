#include <tetris.h>

extern int status;

extern int top;
extern int score;
extern int level;
extern int lines;

extern int next_type;
extern Tetromino player;
extern Board board;

extern int maps[7][4][4];

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