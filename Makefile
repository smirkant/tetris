all:tetris

tetris:main.c display.c tetris.c
	gcc -O -Wall -o tetris main.c display.c tetris.c `pkg-config --cflags --libs gtk+-2.0` -lrt


clean:
	rm tetris
