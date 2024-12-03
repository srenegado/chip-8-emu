main: main.c
	gcc main.c chip8.c -o chip8 `sdl2-config --cflags --libs` -lm

clean:
	rm -f chip8