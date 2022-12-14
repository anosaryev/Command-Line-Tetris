CFLAGS := -lncurses -lpthread

all: tetris.c
	gcc tetris.c -o tetris.out $(CFLAGS)

run: all
	./tetris.out

