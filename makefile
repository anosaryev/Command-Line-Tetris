CFLAGS := -I. -lncurses -lpthread

all: tetris display board player
	gcc tetris.o display.o board.o player.o -o tetris.out $(CFLAGS)

tetris: tetris.c tetris.h display.h board.h
	gcc -c tetris.c $(CFLAGS)

display: display.c display.h
	gcc -c display.c $(CFLAGS)

board: board.c board.h display.h
	gcc -c board.c $(CFLAGS)

player: player.c player.h display.h board.h
	gcc -c player.c $(CFLAGS)

run: all
	./tetris.out

clean:
	rm -f *.o
	rm -f tetris.out
