#include <stdio.h>
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

int maps[7][4][4] = 
{
    { /* I */
        {0, 1, 0, 0},
        {0, 1, 0, 0},
        {0, 1, 0, 0},
        {0, 1, 0, 0}
    }, { /* L */
        {0, 1, 0, 0},
        {0, 1, 0, 0},
        {0, 1, 1, 0},
        {0, 0, 0, 0} 
    }, { /* J */
        {0, 0, 1, 0},
        {0, 0, 1, 0},
        {0, 1, 1, 0},
        {0, 0, 0, 0}
    }, { /* T */
        {0, 0, 0, 0},
        {1, 1, 1, 0},
        {0, 1, 0, 0},
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
} tetromino;

typedef struct pieces {
    int len;
    tetromino data[];
} board;

int next_type;
tetromino player;
board buildup;

int print_tetromino(tetromino p1){
    for (int i = 0; i < 4; i++){
        for (int j = 0; j < 4; j++){
            printf("%d", p1.map[i][j]);
        }
        printf("\n");
    }
}

// colour guide:
// https://dev.to/tenry/terminal-colors-in-c-c-3dgc
void print_tile(tetromino curr){
    switch (curr.type){
        case (I_PIECE):
            printf("\033[36;46m  \033[0m");
            break;
        case (L_PIECE):
            printf("\033[33;43m  \033[0m");
            break;
        case (J_PIECE):
            printf("\033[34;44m  \033[0m");
            break;
        case (T_PIECE):
            printf("\033[35;45m  \033[0m");
            break;
        case (O_PIECE):
            printf("\033[37;47m  \033[0m");
            break;
        case (S_PIECE):
            printf("\033[32;42m  \033[0m");
            break;
        case (Z_PIECE):
            printf("\033[31;41m  \033[0m");
            break;
        default:
            printf("bagel falafel eggs and mac and cheese");
            break;
    }
}

int print_game(){
    system("clear");
    printf("         Command Line Tetris\n\n");
    printf("    Top:    %d\n", 0); // and next tetromino component
    printf("    Score:  %d\n", 0); // and next tetromino component
    printf("    Level:  %d\n", 0); // and next tetromino component
    printf("    Lines:  %d\n", 0); // and next tetromino component
    printf("\n");
    
    int x, y, i;
    for (y = 19; y >= 0; y--){
        printf("      %02d   ", y);
        
        for (x = 0; x < 10; x++){
            // printf("%d", x);
            // search player piece for something at coords
            if (x - player.x < 4 && player.y - y < 4){
                if (player.map[x - player.x][player.y - y] == 1){
                    // draw player piece with colour based on type
                    print_tile(player);
                }else{
                    printf(". ");
                }
            }else{

                // search every board piece for something at coords
                for (i = 0; i < buildup.len; i++){
                    tetromino temp = buildup.data[i];
                    if (x - temp.x < 4 && temp.y - y < 4){
                        if (temp.map[x - temp.x][temp.y - y] == 1){
                            // draw board piece with colour based on type
                            print_tile(temp);
                            break;
                        }
                    }
                }
                
                // no player or board piece
                if (i == buildup.len){
                    printf(". ");
                }
            }
        }
        printf("\n");
    }
    printf("\n           0 1 2 3 4 5 6 7 8 9\n");
}

int main(){
    system("clear");
    srand(time(0));

    player.type = rand()%7;
    player.x = 3;
    player.y = 19;
    memcpy(player.map, maps[player.type], sizeof(int[4][4]));

    // print_tetromino(p1);
    int i = 0;
    while (++i){
        print_game();
        printf("%d\n", i);

        struct timespec t0 = {0, 100000000};
        nanosleep(&t0, NULL);
    }

    return 1;
}

/*
         Command Line Tetris

    Top:    w           [````````]
    Score:  x           [  next  ]
    Level:  y           tetromino]
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