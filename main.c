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
    //     printf("mem[0x%03x] = 0x%02x\n", i, mem[i]);

    unsigned char Vx[16] = {0}; // variable registers: V0 to VF
    unsigned char I[2] = {0}; // index register
    unsigned char delay_timer = {0};
    unsigned char sound_timer = {0};
    unsigned short PC = 0x200; // program counter
    unsigned char stack[32] = {0}; // for storing up to 16 addresses

    // unsigned char lsb = PC & 0xFF;   
    // unsigned char msb = (PC >> 8) & 0xF;

    // printf("program counter = 0x%03x\n", PC);
    // printf("msb = 0x%02x\n", msb);  
    // printf("lsb = 0x%02x\n", lsb);  

    int cpu_freq = 500; // instructions per second (1 instruction ~ 1 cycle)
    int refresh_rate = 60; // frames per second
    int instrs_per_frame = cpu_freq / refresh_rate;

    bool running = true;
    while (running) {

        SDL_Event event;
        char user_pressed_key = '\0';

        while (SDL_PollEvent(&event)) { // read event queue
            switch (event.type) {

                case SDL_KEYDOWN: // user pressed a key

                    switch (event.key.keysym.scancode) { // read key
                        case 30:
                            user_pressed_key = '1';
                            break;
                        case 31:
                            user_pressed_key = '2';
                            break;
                        case 32:
                            user_pressed_key = '3';
                            break;
                        case 33:
                            user_pressed_key = 'C';
                            break;
                        case 20:
                            user_pressed_key = '4';
                            break;
                        case 26:
                            user_pressed_key = '5';
                            break;
                        case 8:
                            user_pressed_key = '6';
                            break;
                        case 21:
                            user_pressed_key = 'D';
                            break;
                        case 4:
                            user_pressed_key = '7';
                            break;
                        case 22:
                            user_pressed_key = '8';
                            break;
                        case 7:
                            user_pressed_key = '9';
                            break;
                        case 9:
                            user_pressed_key = 'E';
                            break;
                        case 29:
                            user_pressed_key = 'A';
                            break;
                        case 27:
                            user_pressed_key = '0';
                            break;
                        case 6:
                            user_pressed_key = 'B';
                            break;
                        case 25:
                            user_pressed_key = 'F';
                            break;
                        case 41: // esc
                            goto quit;
                            break;
                    }
            
                    break;
                
                default:
                    break;
                    
            }
        }

        for (int i = 0; i < instrs_per_frame; i++) {
            // fetch
            printf("Fetch instruction!\n");


            // decode
            printf("Decode instruction!\n");

            // execute
            printf("Execute instruction!\n");
        }

        // update display
        printf("\nUpdating display...\n\n");
    }

quit:
    SDL_Quit();
    return 0;
}