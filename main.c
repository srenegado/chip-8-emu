#include <SDL.h>
#include <stdbool.h>
#include <stdio.h>

int main() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

    SDL_CreateWindow("Chip-8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 64 * 16, 32 * 16, 0);

    char mem[4096] = {0}; // main memory
    char Vx[16] = {0}; // variable registers: V0 to VF
    char I[2] = {0}; // index register
    char delay_timer = {0};
    char sound_timer = {0};
    char PC[2] = {0}; // program counter
    char stack[32] = {0}; // for storing up to 16 addresses

    while (true) {
        SDL_Event event;
        
        while (SDL_PollEvent(&event)) { // read event queue
            switch (event.type) {

                case SDL_KEYDOWN:
                    if (event.key.keysym.scancode == 20) // 'q' scancode is 20
                        goto quit;
                
                default:
                    break;
                    
            }
        }

    }

quit:
    SDL_Quit();
    return 0;
}