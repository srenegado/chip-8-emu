#include <SDL.h>
#include <SDL_mixer.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>

#define DISPLAY_WIDTH_PX 64
#define DISPLAY_HEIGHT_PX 32
#define SCALE 16

int main(int argc, char** argv) {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO);

    srand(time(NULL)); // for seeding

    SDL_Window* display = SDL_CreateWindow("Chip-8", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
        DISPLAY_WIDTH_PX * SCALE, DISPLAY_HEIGHT_PX * SCALE, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(display, -1, SDL_RENDERER_ACCELERATED);
    unsigned char display_arr[DISPLAY_HEIGHT_PX][DISPLAY_WIDTH_PX] = {0};

    // Load "beep" sound effect for sound timer
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048); // stereo, 44100 Hz
    Mix_Chunk* sound_timer_beep = Mix_LoadWAV("audio/microwave_beep.wav");

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
    FILE* rom_file = NULL;
    rom_file = fopen(argv[1], "rb");
    fread(start_addr, 4096 - 512, 1, rom_file);
    fclose(rom_file);

    unsigned char Vx[16] = {0}; // variable registers: V0 to VF
    unsigned short I = 0x000; // index register
    unsigned char delay_timer = {0};
    unsigned char sound_timer = {0};
    unsigned short PC = 0x200; // program counter
    unsigned short stack[16] = {0}; // for storing up to 16 addresses
    unsigned char stack_ptr = 0;

    int cpu_freq = 1000; // instructions per second (1 instruction ~ 1 cycle)
    int refresh_rate = 60; // frames per second
    int frame_ms = 1000 / refresh_rate; // time (ms) per frame
    int instr_per_frame = cpu_freq / refresh_rate;

    unsigned char keyboard[16] = {0}; // index = CHIP-8 key

    bool expecting_release = false; // for FX0A
    unsigned char expecting_key;

    // Main loop
    bool running = true;
    while (running) {
        uint32_t start_ms = SDL_GetTicks();

        bool draw_flag = false;

        // Process user key presses
        SDL_Event event;
        while (SDL_PollEvent(&event)) { 
            switch (event.type) {

                case SDL_KEYDOWN: // user pressed a key

                    switch (event.key.keysym.scancode) { // read key
                        case 30:
                            keyboard[0x1] = 1;
                            break;
                        case 31: 
                            keyboard[0x2] = 1;
                            break;
                        case 32: 
                            keyboard[0x3] = 1;
                            break;
                        case 33:
                            keyboard[0xC] = 1;
                            break;
                        case 20: 
                            keyboard[0x4] = 1;
                            break;
                        case 26: 
                            keyboard[0x5] = 1;
                            break;
                        case 8: 
                            keyboard[0x6] = 1;
                            break;
                        case 21: 
                            keyboard[0xD] = 1;
                            break;
                        case 4:
                            keyboard[0x7] = 1;
                            break;
                        case 22:
                            keyboard[0x8] = 1;
                            break;
                        case 7: 
                            keyboard[0x9] = 1;
                            break;
                        case 9:
                            keyboard[0xE] = 1;
                            break;
                        case 29: 
                            keyboard[0xA] = 1;
                            break;
                        case 27: 
                            keyboard[0x0] = 1;
                            break;
                        case 6: 
                            keyboard[0xB] = 1;
                            break;
                        case 25: 
                            keyboard[0xF] = 1;
                            break;
                        case 41: // esc
                            goto quit;
                            break;
                    }

                    break;

                case SDL_KEYUP: // user released a key

                    switch (event.key.keysym.scancode) { // read key
                      case 30:
                            keyboard[0x1] = 0;
                            break;
                        case 31: 
                            keyboard[0x2] = 0;
                            break;
                        case 32: 
                            keyboard[0x3] = 0;
                            break;
                        case 33:
                            keyboard[0xC] = 0;
                            break;
                        case 20: 
                            keyboard[0x4] = 0;
                            break;
                        case 26: 
                            keyboard[0x5] = 0;
                            break;
                        case 8: 
                            keyboard[0x6] = 0;
                            break;
                        case 21: 
                            keyboard[0xD] = 0;
                            break;
                        case 4:
                            keyboard[0x7] = 0;
                            break;
                        case 22:
                            keyboard[0x8] = 0;
                            break;
                        case 7: 
                            keyboard[0x9] = 0;
                            break;
                        case 9:
                            keyboard[0xE] = 0;
                            break;
                        case 29: 
                            keyboard[0xA] = 0;
                            break;
                        case 27: 
                            keyboard[0x0] = 0;
                            break;
                        case 6: 
                            keyboard[0xB] = 0;
                            break;
                        case 25: 
                            keyboard[0xF] = 0;
                            break;
                    }

                    break;
                
                default:
                    break;
                    
            }
        }

        // Fetch-decode-execute cycle
        for (int i = 0; i < instr_per_frame; i++) { 

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
                    // ...ignore 0NNN instruction

                    switch (lsb) {
                        case (0xE0): // 00E0 - clear screen
                            draw_flag = true;
                            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // black
                            SDL_RenderClear(renderer);
                            memset(display_arr, 0, sizeof(display_arr));  
                            break;
                        case (0xEE): // 00EE - return subroutine
                            stack_ptr--;
                            PC = stack[stack_ptr];
                            stack[stack_ptr] = 0;
                            break;
                    }                    
                    break;
                case (0x1): // 1NNN - jump
                    PC = lowest_12_bits;  
                    break;
                case (0x2): // 2NNN - call subroutine
                    stack[stack_ptr] = PC;
                    stack_ptr++;
                    PC = lowest_12_bits;
                case (0x3): // 3XNN - skip conditionally
                    if (Vx[second_nib] == lsb) 
                        PC += 2;
                    break;
                case (0x4): // 4XNN - skip conditionally
                    if (Vx[second_nib] != lsb)
                        PC += 2;
                    break;
                case (0x5): // 5XY0 - skip conditionally
                    if (Vx[second_nib] == Vx[third_nib])
                        PC += 2;
                    break;
                case (0x6): // 6XNN - set
                    Vx[second_nib] = lsb;   
                    break;
                case (0x7): // 7XNN - add
                    Vx[second_nib] += lsb; 
                    break;
                case (0x8):
                    switch (fourth_nib) {
                        case (0x0): // 8XY0 - set
                            Vx[second_nib] = Vx[third_nib];
                            break;
                        case (0x1): // 8XY1 - OR
                            Vx[second_nib] |= Vx[third_nib]; 
                            break;
                        case (0x2): // 8XY2 - AND
                            Vx[second_nib] &= Vx[third_nib];
                            break;
                        case (0x3): // 8XY3 - XOR
                            Vx[second_nib] ^= Vx[third_nib];
                            break;
                        case (0x4): // 8XY4 - ADD
                            unsigned short sum = Vx[second_nib] + Vx[third_nib];
                            Vx[second_nib] = (unsigned char) sum;
                            if (sum > 255)
                                Vx[0xF] = 1;
                            else 
                                Vx[0xF] = 0;
                            break;
                        case (0x5): // 8XY5 - SUBTRACT
                            bool underflow_8XY5 = Vx[third_nib] > Vx[second_nib];
                            Vx[second_nib] -= Vx[third_nib];
                            if (underflow_8XY5) 
                                Vx[0xF] = 0;
                            else  
                                Vx[0xF] = 1;
                            break;
                        case (0x6): // 8XY6 - shift right
                            // Vx[second_nib] = Vx[third_nib];
                            unsigned char bit_8XY6 = Vx[second_nib] & 0x01;
                            Vx[second_nib] >>= 1;
                            Vx[0xF] = bit_8XY6;
                            break;                        
                        case (0x7): // 8XY7 - SUBTRACT
                            bool underflow_8XY7 = Vx[second_nib] > Vx[third_nib];
                            Vx[second_nib] = Vx[third_nib] - Vx[second_nib];
                            if (underflow_8XY7) 
                                Vx[0xF] = 0;
                            else  
                                Vx[0xF] = 1;
                            break;
                        case (0xE): // 8XYE - shift left
                            // Vx[second_nib] = Vx[third_nib];
                            unsigned char bit_8XYE = (Vx[second_nib] & 0x80) >> 7;
                            Vx[second_nib] <<= 1;
                            Vx[0xF] = bit_8XYE;
                            break;
                    }
                    break;
                case (0x9): // 9XY0 - skip conditionally
                    if (Vx[second_nib] != Vx[third_nib])
                        PC += 2;
                    break;
                case (0xA): // ANNN - set I
                    I = lowest_12_bits; 
                    break;
                case (0xB): // BNNN - jump with offset
                    PC = lowest_12_bits + Vx[0x0];
                case (0xC): // CNNN - random
                    Vx[second_nib] = (unsigned char)rand() & lsb;
                    break;
                case (0xD): // DXYN - display
                    
                    draw_flag = true;

                    // Starting position wraps around
                    unsigned char x = Vx[second_nib] % DISPLAY_WIDTH_PX; 
                    unsigned char y = Vx[third_nib] % DISPLAY_HEIGHT_PX;

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
                case (0xE):
                    switch (lsb) {
                        case (0x9E): // 0xEX9E - DOWN
                            if (keyboard[Vx[second_nib] & 0xF] == 1)
                                PC += 2;
                            break;
                        case (0xA1): // 0xEXA1 - UP
                            if (keyboard[Vx[second_nib] & 0xF] == 0)
                                PC += 2;
                            break;
                    }
                    break;
                case (0xF):
                    switch (lsb) {
                        case (0x07): // FX07 - set Vx to timer
                            Vx[second_nib] = delay_timer;
                            break;
                        case (0x0A): // FX0A - GETKEY
                            if (expecting_release) {
                                if (keyboard[expecting_key] == 0) { // released
                                    expecting_release = false;
                                    Vx[second_nib] = i;
                                    break;
                                } else { 
                                    goto wait;
                                }
                            }

                            // Check for any key presses
                            for (int i = 0; i < 16; i++) { 
                                if (keyboard[i] == 1) {
                                    expecting_release = true;
                                    expecting_key = i;
                                    goto wait;   
                                }
                            }
wait:
                            PC -= 2;
                            break;
                        case (0x15): // FX15 - set delay timer
                            delay_timer = Vx[second_nib];
                            break;
                        case (0x18): // FX18 - set sound timer
                            sound_timer = Vx[second_nib];
                            break;
                        case (0x1E): // FX1E - add to I
                            I += Vx[second_nib];
                            break;
                        case (0x29): // FX29 - font
                            unsigned char font_char = Vx[second_nib] & 0xF;
                            unsigned char offset_FX29 = 0;
                            while (offset_FX29 < font_char)
                                offset_FX29 += 5;
                            I = mem[offset_FX29];
                            break;
                        case (0x33): // FX33 - binary-coded decimal

                            // Hundreds place
                            mem[I] = (unsigned char)
                                floor(Vx[second_nib] / 100) % 10;
                            
                            // Tens place
                            mem[I + 1] = (unsigned char)
                                floor(Vx[second_nib] / 10) % 10; 
                            
                            // Ones place
                            mem[I + 2] = Vx[second_nib] % 10;

                            break;
                        case (0x55): // FX55 - store memory
                            for (int i = 0; i <= second_nib; i++) 
                                mem[I + i] = Vx[i];
                            break;
                        case (0x65): // FX65 - load memory
                            for (int i = 0; i <= second_nib; i++)
                                Vx[i] = mem[I + i];
                            break;
                    }
                    break;
            }
        }

        /*
         * Due to how SDL_RenderPresent() is implemented,
         * have to redraw and render the whole display for each frame.
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

        uint32_t end_ms = SDL_GetTicks();
        uint32_t time_taken_ms = end_ms - start_ms;
        
        if (time_taken_ms < frame_ms) {
            uint32_t delay_ms = frame_ms - time_taken_ms;
            SDL_Delay(delay_ms);
        } 
        
        if (delay_timer > 0) 
            delay_timer--;

        if (sound_timer > 0) {
            Mix_PlayChannel(-1, sound_timer_beep, 0);
            sound_timer--;
        }

    }

quit:
    SDL_Quit();
    return 0;
}