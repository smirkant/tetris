all:tetris

tetris:main.c display.c tetris.c
	gcc -O -g -o tetris main.c display.c tetris.c `pkg-config --cflags --libs gtk+-2.0` -lrt

display:display.o
	gcc display.c `pkg-config --cflags --libs gtk+-2.0`

clean:
	rm tetris
