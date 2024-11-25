#include <SDL.h>
#include <SDL_scancode.h>
#include <SDL_mixer.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>

#define DISPLAY_WIDTH_PX 64
#define DISPLAY_HEIGHT_PX 32
#define SCALE 16


typedef struct display_specs {
    SDL_Window* window;
    SDL_Renderer* renderer;
    uint8_t bits[DISPLAY_HEIGHT_PX][DISPLAY_WIDTH_PX];
} display_specs;


typedef struct chip8_specs {
    uint8_t keyboard[16];
    bool running;
    uint8_t instr_per_frame; 
    uint8_t mem[4096];       // Main memory
    uint16_t PC;             // Program counter
    display_specs display;   
    uint16_t stack[16];      // Stack: stores up to 16 addresses
    int8_t SP;               // Stack pointer
    // draw_flag
    // SP
    // Vx
    // I
    // delay_timer
    // sound_timer
    // expecting key
    // expecting release
} chip8_specs;

// struct keyboard 
//   pressed
//  expecting key
// expecting release
/**
 * Read and store the key the user has pressed or released 
 */
void process_keyboard(chip8_specs* chip8) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) { 
        switch (event.type) {

            case SDL_KEYDOWN: // User pressed a key

                switch (event.key.keysym.scancode) { // Read and store key
                    case SDL_SCANCODE_1:
                        chip8->keyboard[0x1] = 1;
                        break;
                    case SDL_SCANCODE_2: 
                        chip8->keyboard[0x2] = 1;
                        break;
                    case SDL_SCANCODE_3: 
                        chip8->keyboard[0x3] = 1;
                        break;
                    case SDL_SCANCODE_4:
                        chip8->keyboard[0xC] = 1;
                        break;
                    case SDL_SCANCODE_Q: 
                        chip8->keyboard[0x4] = 1;
                        break;
                    case SDL_SCANCODE_W: 
                        chip8->keyboard[0x5] = 1;
                        break;
                    case SDL_SCANCODE_E: 
                        chip8->keyboard[0x6] = 1;
                        break;
                    case SDL_SCANCODE_R: 
                        chip8->keyboard[0xD] = 1;
                        break;
                    case SDL_SCANCODE_A:
                        chip8->keyboard[0x7] = 1;
                        break;
                    case SDL_SCANCODE_S:
                        chip8->keyboard[0x8] = 1;
                        break;
                    case SDL_SCANCODE_D: 
                        chip8->keyboard[0x9] = 1;
                        break;
                    case SDL_SCANCODE_F:
                        chip8->keyboard[0xE] = 1;
                        break;
                    case SDL_SCANCODE_Z: 
                        chip8->keyboard[0xA] = 1;
                        break;
                    case SDL_SCANCODE_X: 
                        chip8->keyboard[0x0] = 1;
                        break;
                    case SDL_SCANCODE_C: 
                        chip8->keyboard[0xB] = 1;
                        break;
                    case SDL_SCANCODE_V: 
                        chip8->keyboard[0xF] = 1;
                        break;
                    case 41: // esc
                        chip8->running = false;
                        break;
                }

                break;

            case SDL_KEYUP: // User released a key

                switch (event.key.keysym.scancode) { // Read and store key
                    case SDL_SCANCODE_1:
                        chip8->keyboard[0x1] = 0;
                        break;
                    case SDL_SCANCODE_2: 
                        chip8->keyboard[0x2] = 0;
                        break;
                    case SDL_SCANCODE_3: 
                        chip8->keyboard[0x3] = 0;
                        break;
                    case SDL_SCANCODE_4:
                        chip8->keyboard[0xC] = 0;
                        break;
                    case SDL_SCANCODE_Q: 
                        chip8->keyboard[0x4] = 0;
                        break;
                    case SDL_SCANCODE_W: 
                        chip8->keyboard[0x5] = 0;
                        break;
                    case SDL_SCANCODE_E: 
                        chip8->keyboard[0x6] = 0;
                        break;
                    case SDL_SCANCODE_R: 
                        chip8->keyboard[0xD] = 0;
                        break;
                    case SDL_SCANCODE_A:
                        chip8->keyboard[0x7] = 0;
                        break;
                    case SDL_SCANCODE_S:
                        chip8->keyboard[0x8] = 0;
                        break;
                    case SDL_SCANCODE_D: 
                        chip8->keyboard[0x9] = 0;
                        break;
                    case SDL_SCANCODE_F:
                        chip8->keyboard[0xE] = 0;
                        break;
                    case SDL_SCANCODE_Z: 
                        chip8->keyboard[0xA] = 0;
                        break;
                    case SDL_SCANCODE_X: 
                        chip8->keyboard[0x0] = 0;
                        break;
                    case SDL_SCANCODE_C: 
                        chip8->keyboard[0xB] = 0;
                        break;
                    case SDL_SCANCODE_V: 
                        chip8->keyboard[0xF] = 0;
                        break;
                }

                break;
            
            default:
                break;
                
        }
    }

    return;
}


void fetch_decode_execute() {
    
    
    return;
}


int main(int argc, char** argv) {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO);

    srand(time(NULL)); // for seeding

    chip8_specs chip8;

    chip8.display.window = SDL_CreateWindow("Chip-8", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
        DISPLAY_WIDTH_PX * SCALE, DISPLAY_HEIGHT_PX * SCALE, 0);
    chip8.display.renderer = SDL_CreateRenderer(chip8.display.window, -1, SDL_RENDERER_ACCELERATED);
    memset(chip8.display.bits, 0, sizeof(chip8.display.bits));


    // Load "beep" sound effect for sound timer
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048); // stereo, 44100 Hz
    Mix_Chunk* sound_timer_beep = Mix_LoadWAV("audio/microwave_beep.wav");

    // Store font in first 512 bytes of memory
    memset(chip8.mem, 0, sizeof(chip8.mem));
    uint8_t font[80] = {
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
    }; 
    memcpy(&chip8.mem[0], font, sizeof(font));

    // Load ROM into memory
    uint16_t start = 0x200;
    uint8_t* start_addr = &chip8.mem[start];
    FILE* rom_file = NULL;
    rom_file = fopen(argv[1], "rb");
    fread(start_addr, 4096 - 512, 1, rom_file);
    fclose(rom_file);

    uint8_t Vx[16] = {0}; // variable registers: V0 to VF
    uint16_t I = 0x000; // index register
    uint8_t delay_timer = {0};
    uint8_t sound_timer = {0};
    chip8.PC = 0x200;
    memset(chip8.stack, 0 ,sizeof(chip8.stack));
    chip8.SP = -1;

    int cpu_freq = 500; // instructions per second (1 instruction ~ 1 cycle)
    int refresh_rate = 60; // frames per second
    int frame_ms = 1000 / refresh_rate; // time (ms) per frame
    chip8.instr_per_frame = cpu_freq / refresh_rate;

    memset(chip8.keyboard, 0, sizeof(chip8.keyboard)); // index = CHIP-8 key

    bool expecting_release = false; // for FX0A
    uint8_t expecting_key;

    // Main loop
    chip8.running = true;
    while (chip8.running) {
        uint32_t start_ms = SDL_GetTicks();

        bool draw_flag = false;

        // Process user key presses
        process_keyboard(&chip8);

        // Fetch-decode-execute cycle
        for (int i = 0; i < chip8.instr_per_frame; i++) { 

            // Fetch: copy instruction PC is pointing to
            uint8_t msb = chip8.mem[chip8.PC];
            uint8_t lsb = chip8.mem[chip8.PC + 1]; // NN

            // Extract values for decoding
            uint8_t first_nib = (msb >> 4) & 0xF;
            uint8_t second_nib = msb & 0xF; // X
            uint8_t third_nib = (lsb >> 4) & 0xF; // Y
            uint8_t fourth_nib = lsb & 0xF; // N
            uint16_t lowest_12_bits = ((short)second_nib << 8) | (short)lsb; // NNN

            chip8.PC += 2; // next instruction

            // Decode and execute
            switch (first_nib) {

                case (0x0):
                    // ...ignore 0NNN instruction

                    switch (lsb) {
                        case (0xE0): // 00E0 - clear screen
                            draw_flag = true;
                            SDL_SetRenderDrawColor(chip8.display.renderer, 0, 0, 0, 255); // black
                            SDL_RenderClear(chip8.display.renderer);
                            memset(chip8.display.bits, 0, sizeof(chip8.display.bits));  
                            break;
                        case (0xEE): // 00EE - return subroutine
                            chip8.PC = chip8.stack[chip8.SP];
                            chip8.SP--;
                            break;
                    }                    
                    break;
                case (0x1): // 1NNN - jump
                    chip8.PC = lowest_12_bits;  
                    break;
                case (0x2): // 2NNN - call subroutine
                    chip8.SP++;
                    chip8.stack[chip8.SP] = chip8.PC;
                    chip8.PC = lowest_12_bits;
                    break;
                case (0x3): // 3XNN - skip conditionally
                    if (Vx[second_nib] == lsb) 
                        chip8.PC += 2;
                    break;
                case (0x4): // 4XNN - skip conditionally
                    if (Vx[second_nib] != lsb)
                        chip8.PC += 2;
                    break;
                case (0x5): // 5XY0 - skip conditionally
                    if (Vx[second_nib] == Vx[third_nib])
                        chip8.PC += 2;
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
                            // Vx[0xF] = 0; // COSMAC VIP feature 
                            break;
                        case (0x2): // 8XY2 - AND
                            Vx[second_nib] &= Vx[third_nib];
                            // Vx[0xF] = 0; // COSMAC VIP feature
                            break;
                        case (0x3): // 8XY3 - XOR
                            Vx[second_nib] ^= Vx[third_nib];
                            // Vx[0xF] = 0; // COSMAC VIP feature
                            break;
                        case (0x4): // 8XY4 - ADD
                            uint16_t sum = Vx[second_nib] + Vx[third_nib];
                            Vx[second_nib] = (uint8_t) sum;
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
                            // Vx[second_nib] = Vx[third_nib]; // COSMAC VIP feature
                            uint8_t bit_8XY6 = Vx[second_nib] & 0x01;
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
                            // Vx[second_nib] = Vx[third_nib]; // COSMAC VIP feature
                            uint8_t bit_8XYE = (Vx[second_nib] & 0x80) >> 7;
                            Vx[second_nib] <<= 1;
                            Vx[0xF] = bit_8XYE;
                            break;
                    }
                    break;
                case (0x9): // 9XY0 - skip conditionally
                    if (Vx[second_nib] != Vx[third_nib])
                        chip8.PC += 2;
                    break;
                case (0xA): // ANNN - set I
                    I = lowest_12_bits; 
                    break;
                case (0xB): // BNNN - jump with offset
                    chip8.PC = lowest_12_bits + Vx[0x0];
                    break;
                case (0xC): // CNNN - random
                    Vx[second_nib] = (uint8_t)rand() & lsb;
                    break;
                case (0xD): // DXYN - display
                    
                    draw_flag = true;

                    // Starting position wraps around
                    uint8_t x = Vx[second_nib] % DISPLAY_WIDTH_PX; 
                    uint8_t y = Vx[third_nib] % DISPLAY_HEIGHT_PX;

                    Vx[0xF] = 0; // turn off collision

                    for (int i = 0; i < fourth_nib; i++) { // read N bytes    
                        uint8_t byte = chip8.mem[I + i]; // start from I

                        for (int j = 7; j >= 0; j--) { // read each bit
                            uint8_t bit = (byte & (1 << j)) >> j;

                            if (bit == 1) {
                                if (chip8.display.bits[y][x] == 1) { // collision
                                    chip8.display.bits[y][x] = 0;
                                    Vx[0xF] = 1; 
                                } else { // no collision
                                    chip8.display.bits[y][x] = 1;
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
                            if (chip8.keyboard[Vx[second_nib] & 0xF] == 1)
                                chip8.PC += 2;
                            break;
                        case (0xA1): // 0xEXA1 - UP
                            if (chip8.keyboard[Vx[second_nib] & 0xF] == 0)
                                chip8.PC += 2;
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
                                if (chip8.keyboard[expecting_key] == 0) { // released
                                    expecting_release = false;
                                    Vx[second_nib] = i;
                                    break;
                                } else { 
                                    goto wait;
                                }
                            }

                            // Check for any key presses
                            for (int i = 0; i < 16; i++) { 
                                if (chip8.keyboard[i] == 1) {
                                    expecting_release = true;
                                    expecting_key = i;
                                    goto wait;   
                                }
                            }
                            
                            wait:
                            chip8.PC -= 2;
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
                            I =  Vx[second_nib] * 5;
                            break;
                        case (0x33): // FX33 - binary-coded decimal

                            // Hundreds place
                            chip8.mem[I] = (uint8_t)
                                floor(Vx[second_nib] / 100) % 10;
                            
                            // Tens place
                            chip8.mem[I + 1] = (uint8_t)
                                floor(Vx[second_nib] / 10) % 10; 
                            
                            // Ones place
                            chip8.mem[I + 2] = Vx[second_nib] % 10;

                            break;
                        case (0x55): // FX55 - store memory
                            for (int i = 0; i <= second_nib; i++) 
                                chip8.mem[I + i] = Vx[i];
                            // I += second_nib + 1; // COSMAC VIP feature
                            // I += second_nib; // Chip-48 feature
                            break;
                        case (0x65): // FX65 - load memory
                            for (int i = 0; i <= second_nib; i++)
                                Vx[i] = chip8.mem[I + i];
                            // I += second_nib + 1; // COSMAC VIP feature
                            // I += second_nib; // Chip-48 feature
                            break;
                    }
                    break;
            }

        }

        uint32_t end_ms = SDL_GetTicks();
        uint32_t time_taken_ms = end_ms - start_ms;
        
        if (time_taken_ms < frame_ms) {
            uint32_t delay_ms = frame_ms - time_taken_ms;
            SDL_Delay(delay_ms);
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
                    if (chip8.display.bits[y][x] == 0) {
                        SDL_SetRenderDrawColor(chip8.display.renderer, 
                            0, 0, 0, 255); // black 
                    } else { 
                        SDL_SetRenderDrawColor(chip8.display.renderer, 
                            255, 255, 255, 255); // white
                    }
                    SDL_RenderFillRect(chip8.display.renderer, &px); 
                }
            }

            // Update display
            SDL_RenderPresent(chip8.display.renderer);
        }
        
        if (delay_timer > 0) 
            delay_timer--;

        if (sound_timer > 0) {
            Mix_PlayChannel(-1, sound_timer_beep, 0);
            sound_timer--;
        }

    }

    SDL_Quit();
    return 0;
}