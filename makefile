main: main.c
	gcc main.c -o main `sdl2-config --cflags --libs` -I./include -lSDL2_mixer 

clean:
	rm -f main