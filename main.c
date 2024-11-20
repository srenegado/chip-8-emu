#include <SDL.h>
#include <stdbool.h>
#include <stdio.h>

int main() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

    SDL_CreateWindow("Chip-8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 64 * 16, 32 * 16, 0);

    // main memory
    unsigned char mem[4096] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80, // F
    }; // store font in first 512 bytes

    // for (int i = 0; i < 100; i++)
    //     printf("mem[%d] = 0x%02x\n", i, mem[i]);

    unsigned char Vx[16] = {0}; // variable registers: V0 to VF
    unsigned char I[2] = {0}; // index register
    unsigned char delay_timer = {0};
    unsigned char sound_timer = {0};
    unsigned char PC[2] = {0}; // program counter
    unsigned char stack[32] = {0}; // for storing up to 16 addresses

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