#include <SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>

#define DISPLAY_WIDTH_PX 64
#define DISPLAY_HEIGHT_PX 32
#define SCALE 16

int main(int argc, char** argv) {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* display = SDL_CreateWindow("Chip-8", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
        DISPLAY_WIDTH_PX * SCALE, DISPLAY_HEIGHT_PX * SCALE, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(display, -1, SDL_RENDERER_ACCELERATED);
    unsigned char display_arr[DISPLAY_HEIGHT_PX][DISPLAY_WIDTH_PX] = {0};
    
    // Main memory
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

    // Load ROM into memory
    unsigned short start = 0x200;
    char* start_addr = &mem[start];
    FILE* rom_file;
    rom_file = fopen(argv[1], "rb");
    fread(start_addr, 4096 - 512, 1, rom_file);
    fclose(rom_file);

    // printf("0x00: ");
    // for(int i = 0; i < 4096; i++) {
    //     if (i % 16 == 0 && i != 0) {
    //         printf("\n");
    //         printf("0x%02x: ", i);
    //     }
    //     printf("%02x ", mem[i]); // prints a series of bytes
    // }

    unsigned char Vx[16] = {0}; // variable registers: V0 to VF
    unsigned short I = 0x000; // index register
    unsigned char delay_timer = {0};
    unsigned char sound_timer = {0};
    unsigned short PC = 0x200; // program counter
    unsigned short stack[16] = {0}; // for storing up to 16 addresses
    unsigned char stack_ptr = 0;

    int cpu_freq = 500; // instructions per second (1 instruction ~ 1 cycle)
    int refresh_rate = 30; // frames per second
    int instr_per_frame = cpu_freq / refresh_rate;

    bool running = true;
    while (running) {

        SDL_Event event;
        char user_pressed_key = '\0';
        bool draw_flag = false;

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

        for (int i = 0; i < instr_per_frame; i++) { // fetch-decode-execute

            // Fetch: copy instruction PC is pointing to
            unsigned char msb = mem[PC];
            unsigned char lsb = mem[PC + 1]; // NN

            // Extract values for decoding
            unsigned char first_nib = (msb >> 4) & 0xF;
            unsigned char second_nib = msb & 0xF; // X
            unsigned char third_nib = (lsb >> 4) & 0xF; // Y
            unsigned char fourth_nib = lsb & 0xF; // N
            unsigned short lowest_12_bits = ((short)second_nib << 8) | (short)lsb; // NNN

            PC += 2; // next instruction

            // Decode and execute
            switch (first_nib) {

                case (0x0):
                    // Ignore 0NNN instruction
                    // printf("Instruction: Clear\n");

                    if (lowest_12_bits == 0x0E0) { // 00E0 - clear screen
                        draw_flag = true;
                        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // black
                        SDL_RenderClear(renderer);
                        memset(display_arr, 0, sizeof(display_arr));  
                    }
                    
                    break;
                case (0x1): // 1NNN - jump
                    // printf("Instruction: Jump\n");
                    PC = lowest_12_bits;  
                    break;
                // case (0x3): // 3XNN - skip conditionally
                //     // printf("Instruction: skip conditionally");
                //     if (Vx[second_nib] == lsb) 
                //         PC += 2;
                //     break;
                // case (0x4): // 4XNN - skip conditionally
                //     // printf("Instruction: skip conditionally");
                //     if (Vx[second_nib] != lsb)
                //         PC += 2;
                //     break;
                // case (0x5): // 5XY0 - skip conditionally
                //     // printf("Instruction: skip conditionally");
                //     if (Vx[second_nib] == Vx[third_nib])
                //         PC += 2;
                //     break;
                case (0x6): // 6XNN - set
                    // printf("Instruction: Set Vx\n");
                    Vx[second_nib] = lsb;   
                    break;
                case (0x7): // 7XNN - add
                    // printf("Instruction: Add to Vx\n");
                    Vx[second_nib] += lsb; 
                    break;
                // case (0x8):
                //     switch (fourth_nib) {
                //         // case (0x1): // 8XY1 - OR
                //         //     Vx[second_nib] |= Vx[third_nib]; 
                //         //     break;
                //         // case (0x2): // 8XY2 - AND
                //         //     Vx[second_nib] &= Vx[third_nib];
                //         //     break;
                //         // case (0x3): // 8XY3 - XOR
                //         //     Vx[second_nib] ^= Vx[third_nib];
                //         //     break;
                //         // case (0x5): // 8XY5 - subtract
                //         //     if (Vx[second_nib] > Vx[third_nib]) {
                //         //         Vx[second_nib] -= Vx[third_nib];
                //         //         Vx[0xF] = 1;
                //         //     } else { // underflow
                //         //         Vx[second_nib] = 0;
                //         //         Vx[0xF] = 0;
                //         //     }
                //         //     break;
                //         // case (0x6): // 8XY6 - shift right
                //         //     // Vx[second_nib] = Vx[third_nib];
                //         //     Vx[0xF] = Vx[second_nib] & 0x01;
                //         //     Vx[second_nib] >>= 1;
                //         //     break;                        
                //         // case (0x7): // 8XY7 - subtract
                //         //     if (Vx[third_nib] > Vx[second_nib]) {
                //         //         Vx[second_nib] = Vx[third_nib] - Vx[second_nib];
                //         //         Vx[0xF] = 1;
                //         //     } else { // underflow
                //         //         Vx[second_nib] = 0;
                //         //         Vx[0xF] = 0;
                //         //     }
                //         //     break;
                //         // case (0xE): // 8XYE - shift left
                //         //     // Vx[second_nib] = Vx[third_nib];
                //         //     Vx[0xF] = (Vx[second_nib] & 0x80) >> 7;
                //         //     Vx[second_nib] <<= 1;
                //         //     break;
                //     }
                //     break;
                case (0xA): // ANNN - set I
                    // printf("Instruction: Set I\n");
                    I = lowest_12_bits; 
                    break;
                case (0xD): // DXYN - display
                    
                    draw_flag = true;

                    // Starting position wraps around
                    unsigned char x = Vx[second_nib] % DISPLAY_WIDTH_PX; 
                    unsigned char y = Vx[third_nib] % DISPLAY_HEIGHT_PX;
                    // printf("Instruction: Draw at (%d, %d)\n", x, y);

                    Vx[0xF] = 0; // turn off collision

                    for (int i = 0; i < fourth_nib; i++) { // read N bytes    
                        unsigned char byte = mem[I + i]; // start from I

                        for (int j = 7; j >= 0; j--) { // read each bit
                            unsigned char bit = (byte & (1 << j)) >> j;

                            if (bit == 1) {
                                if (display_arr[y][x] == 1) { // collision
                                    display_arr[y][x] = 0;
                                    Vx[0xF] = 1; 
                                } else { // no collision
                                    display_arr[y][x] = 1;
                                }
                            } 

                            x++;
                            if (x >= DISPLAY_WIDTH_PX) // stop
                                break;
                        }

                        // Reset x for next row
                        x = Vx[second_nib] % DISPLAY_WIDTH_PX; 

                        y++;
                        if (y >= DISPLAY_HEIGHT_PX) // stop
                            break;
                    }    

                    break;
                // case (0xF):
                //     switch (lsb) {
                //         // case (0x33): // FX33 - binary-coded decimal

                //         //     // Hundreds place
                //         //     mem[I] = (unsigned char)
                //         //         floor(Vx[second_nib] / 100) % 10;
                            
                //         //     // Tens place
                //         //     mem[I + 1] = (unsigned char)
                //         //         floor(Vx[second_nib] / 10) % 10; 
                            
                //         //     // Ones place
                //         //     mem[I + 2] = Vx[second_nib] % 10;

                //         //     break;
                //         // case (0x55): // FX55 - store memory
                //         //     for (int i = 0; i <= second_nib; i++) 
                //         //         mem[I + i] = Vx[i];
                //         //     break;
                //         // case (0x65): // FX65 - load memory
                //         //     for (int i = 0; i <= second_nib; i++)
                //         //         Vx[i] = mem[I + i];
                //         //     break;
                //     }
                //     break;
            }
        }

        /*
         * Due to how SDL_RenderPresent() is implemented,
         * only call it following a drawing instruction
         * https://wiki.libsdl.org/SDL2/SDL_RenderPresent
         **/ 
        if (draw_flag) {
            draw_flag = false;

            // Render each pixel
            for (int y = 0; y < DISPLAY_HEIGHT_PX; y++) {
                for (int x = 0; x < DISPLAY_WIDTH_PX; x++) {
                    SDL_Rect px; // create "pixel" to-scale
                    px.x = x * SCALE;
                    px.y = y * SCALE;
                    px.w = SCALE;
                    px.h = SCALE;
                    if (display_arr[y][x] == 0) {
                        SDL_SetRenderDrawColor(renderer, 
                            0, 0, 0, 255); // black 
                    } else { 
                        SDL_SetRenderDrawColor(renderer, 
                            255, 255, 255, 255); // white
                    }
                    SDL_RenderFillRect(renderer, &px); 
                }
            }

            // Update display
            SDL_RenderPresent(renderer);
        }

        
    }

quit:
    SDL_Quit();
    return 0;
}