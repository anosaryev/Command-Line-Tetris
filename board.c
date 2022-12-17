#include <tetris.h>
#include <display.h>

extern int status;
extern int delay;

extern int top;
extern int score;
extern int level;
extern int lines;

extern int next_type;
extern Tetromino player;
extern Board board;

int bag_idx = 0;
int bag[7] = {0, 1, 2, 3, 4, 5, 6};

extern int maps[7][4][4];

int choose_piece(){
    if (bag_idx == 0){
        for (int i = 0; i < 7; i++){
            int swap = rand() % 7;
            int temp = bag[i];
            bag[i] = bag[swap];
            bag[swap] = temp;
        }
    }
    
    bag_idx ++;
    bag_idx %= 7;
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
    if (f){
        char c = fgetc(f);
        while (c != EOF) {
            n = n*10 + atoi(&c);
            c = fgetc(f);
        }
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
        // fputs("\n", f);
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
    }

    if (lines/10 > level){
        level++;
        delay *= 0.8;
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
    set_top();
    print_board();
    update_board();
    mvprintw(y++, x+8, "Game over!\n");
    mvprintw(y++, x+2, "Press any key to quit.\n");
    getch();

    top = set_top();
    endwin();
    exit(0);
}