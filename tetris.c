#include <tetris.h>
#include <display.h>
#include <board.h>
#include <player.h>

int status = 1;
int delay = 999999999;

int top = 0;
int score = 0;
int level = 0;
int lines = 0;

int next_type;
Tetromino player;
Board board = {0, {}};

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

void *move_down_passive(){
    while (status){
        struct timespec t0 = {0, delay};
        nanosleep(&t0, NULL);
        move_down();
        update_board();
        refresh();
    }
}

int main(){
    srand(time(0));
    
    initscr();
    noecho();
    refresh();

    clear();
    cbreak();
    curs_set(0);
    keypad(stdscr, TRUE);
    
    init_colors();
    
    top = get_top();
    next_type = choose_piece();
    next_piece();

    int q;
    print_board();
    update_board();
    print_controls();
    q = getch();
 
    // moving down thread
    pthread_t down_id;
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