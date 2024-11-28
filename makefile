main: main.c
	gcc main.c chip8.c -o chip8 `sdl2-config --cflags --libs` -I./include -lSDL2_mixer -lm

clean:
	rm -f chip8