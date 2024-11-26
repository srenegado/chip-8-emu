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


typedef struct Display {
    SDL_Window* window;
    SDL_Renderer* renderer;
    uint8_t bits[DISPLAY_HEIGHT_PX][DISPLAY_WIDTH_PX];
    bool draw_flag;
} Display;


typedef struct Keyboard {
    uint8_t pressed[16];
    uint8_t expecting_key;
    bool expecting_release;
} Keyboard;


typedef struct Chip8 {
    uint8_t mem[4096];       // Main memory
    uint8_t Vx[16];          // General registers: V0 to VF
    uint16_t I;              // Index register I stores an address
    uint8_t delay_timer;     
    uint8_t sound_timer;
    uint16_t PC;             // Program counter
    uint16_t stack[16];      // Stack: stores up to 16 addresses
    int8_t SP;               // Stack pointer
    Keyboard keyboard;
    Display display;
    uint8_t instr_per_frame;
    bool running; 
} Chip8;


/**
 * Read and store the key the user has pressed or released 
 */
void process_keyboard(Chip8* chip8) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) { 
        switch (event.type) {

            case SDL_KEYDOWN: // User pressed a key

                switch (event.key.keysym.scancode) { // Read and store key
                    case SDL_SCANCODE_1:
                        chip8->keyboard.pressed[0x1] = 1;
                        break;
                    case SDL_SCANCODE_2: 
                        chip8->keyboard.pressed[0x2] = 1;
                        break;
                    case SDL_SCANCODE_3: 
                        chip8->keyboard.pressed[0x3] = 1;
                        break;
                    case SDL_SCANCODE_4:
                        chip8->keyboard.pressed[0xC] = 1;
                        break;
                    case SDL_SCANCODE_Q: 
                        chip8->keyboard.pressed[0x4] = 1;
                        break;
                    case SDL_SCANCODE_W: 
                        chip8->keyboard.pressed[0x5] = 1;
                        break;
                    case SDL_SCANCODE_E: 
                        chip8->keyboard.pressed[0x6] = 1;
                        break;
                    case SDL_SCANCODE_R: 
                        chip8->keyboard.pressed[0xD] = 1;
                        break;
                    case SDL_SCANCODE_A:
                        chip8->keyboard.pressed[0x7] = 1;
                        break;
                    case SDL_SCANCODE_S:
                        chip8->keyboard.pressed[0x8] = 1;
                        break;
                    case SDL_SCANCODE_D: 
                        chip8->keyboard.pressed[0x9] = 1;
                        break;
                    case SDL_SCANCODE_F:
                        chip8->keyboard.pressed[0xE] = 1;
                        break;
                    case SDL_SCANCODE_Z: 
                        chip8->keyboard.pressed[0xA] = 1;
                        break;
                    case SDL_SCANCODE_X: 
                        chip8->keyboard.pressed[0x0] = 1;
                        break;
                    case SDL_SCANCODE_C: 
                        chip8->keyboard.pressed[0xB] = 1;
                        break;
                    case SDL_SCANCODE_V: 
                        chip8->keyboard.pressed[0xF] = 1;
                        break;
                    case 41: // esc
                        chip8->running = false;
                        break;
                }

                break;

            case SDL_KEYUP: // User released a key

                switch (event.key.keysym.scancode) { // Read and store key
                    case SDL_SCANCODE_1:
                        chip8->keyboard.pressed[0x1] = 0;
                        break;
                    case SDL_SCANCODE_2: 
                        chip8->keyboard.pressed[0x2] = 0;
                        break;
                    case SDL_SCANCODE_3: 
                        chip8->keyboard.pressed[0x3] = 0;
                        break;
                    case SDL_SCANCODE_4:
                        chip8->keyboard.pressed[0xC] = 0;
                        break;
                    case SDL_SCANCODE_Q: 
                        chip8->keyboard.pressed[0x4] = 0;
                        break;
                    case SDL_SCANCODE_W: 
                        chip8->keyboard.pressed[0x5] = 0;
                        break;
                    case SDL_SCANCODE_E: 
                        chip8->keyboard.pressed[0x6] = 0;
                        break;
                    case SDL_SCANCODE_R: 
                        chip8->keyboard.pressed[0xD] = 0;
                        break;
                    case SDL_SCANCODE_A:
                        chip8->keyboard.pressed[0x7] = 0;
                        break;
                    case SDL_SCANCODE_S:
                        chip8->keyboard.pressed[0x8] = 0;
                        break;
                    case SDL_SCANCODE_D: 
                        chip8->keyboard.pressed[0x9] = 0;
                        break;
                    case SDL_SCANCODE_F:
                        chip8->keyboard.pressed[0xE] = 0;
                        break;
                    case SDL_SCANCODE_Z: 
                        chip8->keyboard.pressed[0xA] = 0;
                        break;
                    case SDL_SCANCODE_X: 
                        chip8->keyboard.pressed[0x0] = 0;
                        break;
                    case SDL_SCANCODE_C: 
                        chip8->keyboard.pressed[0xB] = 0;
                        break;
                    case SDL_SCANCODE_V: 
                        chip8->keyboard.pressed[0xF] = 0;
                        break;
                }

                break;
            
            default:
                break;
                
        }
    }

    return;
}

/**
 * Emulate a CPU instruction cycle
 */
void fetch_decode_execute(Chip8* chip8) {
    
    // Fetch: copy instruction PC is pointing to
    uint8_t msb = chip8->mem[chip8->PC];
    uint8_t lsb = chip8->mem[chip8->PC + 1];                         // NN

    // Extract values for decoding
    uint8_t first_nib = (msb >> 4) & 0xF;
    uint8_t second_nib = msb & 0xF;                                  // X
    uint8_t third_nib = (lsb >> 4) & 0xF;                            // Y
    uint8_t fourth_nib = lsb & 0xF;                                  // N
    uint16_t lowest_12_bits = ((short)second_nib << 8) | (short)lsb; // NNN

    chip8->PC += 2; // Go to next instruction

    // Decode and execute
    switch (first_nib) {

        case (0x0):
            // ...ignore 0NNN instruction

            switch (lsb) {
                case (0xE0): // 00E0 - Clear screen
                    chip8->display.draw_flag = true;
                    SDL_SetRenderDrawColor(chip8->display.renderer, 0, 0, 0, 255); // black
                    SDL_RenderClear(chip8->display.renderer);
                    memset(chip8->display.bits, 0, sizeof(chip8->display.bits));  
                    break;
                case (0xEE): // 00EE - Return from subroutine
                    chip8->PC = chip8->stack[chip8->SP];
                    chip8->SP--;
                    break;
            }                    
            break;
        case (0x1): // 1NNN - Jump to NNN
            chip8->PC = lowest_12_bits;  
            break;
        case (0x2): // 2NNN - Call subroutine
            chip8->SP++;
            chip8->stack[chip8->SP] = chip8->PC;
            chip8->PC = lowest_12_bits;
            break;
        case (0x3): // 3XNN - Skip if VX == NN
            if (chip8->Vx[second_nib] == lsb) 
                chip8->PC += 2;
            break;
        case (0x4): // 4XNN - Skip if VX != NN
            if (chip8->Vx[second_nib] != lsb)
                chip8->PC += 2;
            break;
        case (0x5): // 5XY0 - Skip if VX == VY
            if (chip8->Vx[second_nib] == chip8->Vx[third_nib])
                chip8->PC += 2;
            break;
        case (0x6): // 6XNN - Set VX = NN
            chip8->Vx[second_nib] = lsb;   
            break;
        case (0x7): // 7XNN - Add VX += NN
            chip8->Vx[second_nib] += lsb; 
            break;
        case (0x8):
            switch (fourth_nib) {
                case (0x0): // 8XY0 - Set VX = VY
                    chip8->Vx[second_nib] = chip8->Vx[third_nib];
                    break;
                case (0x1): // 8XY1 - OR VX |= VY
                    chip8->Vx[second_nib] |= chip8->Vx[third_nib];
                    // chip8->Vx[0xF] = 0; // COSMAC VIP feature 
                    break;
                case (0x2): // 8XY2 - AND VX &= VY
                    chip8->Vx[second_nib] &= chip8->Vx[third_nib];
                    // chip8->Vx[0xF] = 0; // COSMAC VIP feature
                    break;
                case (0x3): // 8XY3 - XOR VX ^= VY
                    chip8->Vx[second_nib] ^= chip8->Vx[third_nib];
                    // chip8->Vx[0xF] = 0; // COSMAC VIP feature
                    break;
                case (0x4): // 8XY4 - ADD VX += VY
                    uint16_t sum = chip8->Vx[second_nib] + chip8->Vx[third_nib];
                    chip8->Vx[second_nib] = (uint8_t) sum;
                    if (sum > 255)
                        chip8->Vx[0xF] = 1;
                    else 
                        chip8->Vx[0xF] = 0;
                    break;
                case (0x5): // 8XY5 - SUBTRACT VX -= VY
                    bool underflow_8XY5 = chip8->Vx[third_nib] > chip8->Vx[second_nib];
                    chip8->Vx[second_nib] -= chip8->Vx[third_nib];
                    if (underflow_8XY5) 
                        chip8->Vx[0xF] = 0;
                    else  
                        chip8->Vx[0xF] = 1;
                    break;
                case (0x6): // 8XY6 - Shift right VX
                    // chip8->Vx[second_nib] = chip8->Vx[third_nib]; // COSMAC VIP feature
                    uint8_t bit_8XY6 = chip8->Vx[second_nib] & 0x01;
                    chip8->Vx[second_nib] >>= 1;
                    chip8->Vx[0xF] = bit_8XY6;
                    break;                        
                case (0x7): // 8XY7 - SUBTRACT VX = VY - VX
                    bool underflow_8XY7 = chip8->Vx[second_nib] > chip8->Vx[third_nib];
                    chip8->Vx[second_nib] = chip8->Vx[third_nib] - chip8->Vx[second_nib];
                    if (underflow_8XY7) 
                        chip8->Vx[0xF] = 0;
                    else  
                        chip8->Vx[0xF] = 1;
                    break;
                case (0xE): // 8XYE - Shift left VX
                    // chip8->Vx[second_nib] = chip8->Vx[third_nib]; // COSMAC VIP feature
                    uint8_t bit_8XYE = (chip8->Vx[second_nib] & 0x80) >> 7;
                    chip8->Vx[second_nib] <<= 1;
                    chip8->Vx[0xF] = bit_8XYE;
                    break;
            }
            break;
        case (0x9): // 9XY0 - Skip if VX != VY
            if (chip8->Vx[second_nib] != chip8->Vx[third_nib])
                chip8->PC += 2;
            break;
        case (0xA): // ANNN - Set I = NNN
            chip8->I = lowest_12_bits; 
            break;
        case (0xB): // BNNN - Jump to (NNN + V0)
            chip8->PC = lowest_12_bits + chip8->Vx[0x0];
            break;
        case (0xC): // CNNN - Set VX = random_number & NN
            chip8->Vx[second_nib] = (uint8_t)rand() & lsb;
            break;
        case (0xD): // DXYN - Draw onto the display
            
            chip8->display.draw_flag = true;

            // Starting position wraps around
            uint8_t x = chip8->Vx[second_nib] % DISPLAY_WIDTH_PX; 
            uint8_t y = chip8->Vx[third_nib] % DISPLAY_HEIGHT_PX;

            chip8->Vx[0xF] = 0; // Turn off collision

            // Read N bytes starting at address I
            for (int i = 0; i < fourth_nib; i++) {    
                uint8_t byte = chip8->mem[chip8->I + i];

                // Read each bit in each byte from right-to-left
                for (int j = 7; j >= 0; j--) { 
                    uint8_t bit = (byte & (1 << j)) >> j;

                    // XOR bits onto the screen
                    if (bit == 1) {
                        if (chip8->display.bits[y][x] == 1) { 
                            chip8->display.bits[y][x] = 0;
                            chip8->Vx[0xF] = 1; 
                        } else { 
                            chip8->display.bits[y][x] = 1;
                        }
                    } 

                    x++;
                    if (x >= DISPLAY_WIDTH_PX) // STOP
                        break;
                }

                // Reset x for next row
                x = chip8->Vx[second_nib] % DISPLAY_WIDTH_PX; 

                y++;
                if (y >= DISPLAY_HEIGHT_PX) // STOP
                    break;
            }    

            break;
        case (0xE):
            switch (lsb) {
                case (0x9E): // 0xEX9E - Skip if key in VX is pressed
                    if (chip8->keyboard.pressed[chip8->Vx[second_nib] & 0xF] == 1)
                        chip8->PC += 2;
                    break;
                case (0xA1): // 0xEXA1 - Skip if key in VX is released
                    if (chip8->keyboard.pressed[chip8->Vx[second_nib] & 0xF] == 0)
                        chip8->PC += 2;
                    break;
            }
            break;
        case (0xF):
            switch (lsb) {
                case (0x07): // FX07 - Set Vx to timer
                    chip8->Vx[second_nib] = chip8->delay_timer;
                    break;
                case (0x0A): // FX0A - Wait for key input
                    if (chip8->keyboard.expecting_release) {
                        if (chip8->keyboard.pressed[chip8->keyboard.expecting_key] == 0) {
                            chip8->keyboard.expecting_release = false;
                            chip8->Vx[second_nib] = chip8->keyboard.expecting_key;
                            break;
                        } else { 
                            goto wait;
                        }
                    }

                    // Check for any key presses
                    for (int i = 0; i < 16; i++) { 
                        if (chip8->keyboard.pressed[i] == 1) {
                            chip8->keyboard.expecting_release = true;
                            chip8->keyboard.expecting_key = i;
                            goto wait;   
                        }
                    }
                    
                    wait:
                    chip8->PC -= 2;
                    break;
                case (0x15): // FX15 - Set delay timer
                    chip8->delay_timer = chip8->Vx[second_nib];
                    break;
                case (0x18): // FX18 - Set sound timer
                    chip8->sound_timer = chip8->Vx[second_nib];
                    break;
                case (0x1E): // FX1E - Add to I
                    chip8->I += chip8->Vx[second_nib];
                    break;
                case (0x29): // FX29 - Set I to font in VX
                    chip8->I =  chip8->Vx[second_nib] * 5;
                    break;
                case (0x33): // FX33 - Store VX as a binary-coded decimal

                    // Hundreds place
                    chip8->mem[chip8->I] = (uint8_t)
                        floor(chip8->Vx[second_nib] / 100) % 10;
                    
                    // Tens place
                    chip8->mem[chip8->I + 1] = (uint8_t)
                        floor(chip8->Vx[second_nib] / 10) % 10; 
                    
                    // Ones place
                    chip8->mem[chip8->I + 2] = chip8->Vx[second_nib] % 10;

                    break;
                case (0x55): // FX55 - Store memory starting from I
                    for (int i = 0; i <= second_nib; i++) 
                        chip8->mem[chip8->I + i] = chip8->Vx[i];
                    // I += second_nib + 1; // COSMAC VIP feature
                    // I += second_nib; // Chip-48 feature
                    break;
                case (0x65): // FX65 - Load memory starting from I
                    for (int i = 0; i <= second_nib; i++)
                        chip8->Vx[i] = chip8->mem[chip8->I + i];
                    // I += second_nib + 1; // COSMAC VIP feature
                    // I += second_nib; // Chip-48 feature
                    break;
            }
            break;
    }

    return;
}


int main(int argc, char** argv) {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO);

    Chip8 chip8;

    // Initialize display
    chip8.display.window = SDL_CreateWindow("Chip-8", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
        DISPLAY_WIDTH_PX * SCALE, DISPLAY_HEIGHT_PX * SCALE, 0);
    chip8.display.renderer = SDL_CreateRenderer(chip8.display.window, -1, SDL_RENDERER_ACCELERATED);
    memset(chip8.display.bits, 0, sizeof(chip8.display.bits));

    // Initialize memory 
    memset(chip8.mem, 0, sizeof(chip8.mem));
    uint8_t font[80] = { // Store font in fire 512 bytes in memory
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

    // Initialize registers
    memset(chip8.Vx, 0, sizeof(chip8.Vx));
    chip8.I = 0x000; 
    chip8.delay_timer = 0;
    chip8.sound_timer = 0;
    chip8.PC = 0x200;
    memset(chip8.stack, 0, sizeof(chip8.stack));
    chip8.SP = -1;

    // Calculate instructions per frame 
    int cpu_freq = 500;                               // instructions per second
    int refresh_rate = 60;                            // frames per second (FPS)
    int frame_ms = 1000 / refresh_rate;               // time (in ms) per frame
    chip8.instr_per_frame = cpu_freq / refresh_rate;

    // Initialize keyboard
    memset(chip8.keyboard.pressed, 0, sizeof(chip8.keyboard.pressed));
    chip8.keyboard.expecting_release = false;

    // Load "beep" sound effect for sound timer
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048); // stereo, 44100 Hz
    Mix_Chunk* sound_timer_beep = Mix_LoadWAV("audio/microwave_beep.wav");

    srand(time(NULL));

    // Main loop
    chip8.running = true;
    while (chip8.running) {
        uint32_t start_ms = SDL_GetTicks(); // Time each frame

        chip8.display.draw_flag = false;

        // Process user key presses
        process_keyboard(&chip8);

        // Each frame should do a fixed number of instructions 
        for (int i = 0; i < chip8.instr_per_frame; i++)  
            fetch_decode_execute(&chip8);
        
        uint32_t end_ms = SDL_GetTicks();
        uint32_t time_taken_ms = end_ms - start_ms;
        
        // Each frame should take a fixed number of seconds
        if (time_taken_ms < frame_ms) {
            uint32_t delay_ms = frame_ms - time_taken_ms;
            SDL_Delay(delay_ms);
        } 

        /*
         * Due to how SDL_RenderPresent() is implemented,
         * have to redraw and render the whole display for each frame.
         * https://wiki.libsdl.org/SDL2/SDL_RenderPresent
         **/ 
        if (chip8.display.draw_flag) {
            chip8.display.draw_flag = false;

            // Render each pixel
            for (int y = 0; y < DISPLAY_HEIGHT_PX; y++) {
                for (int x = 0; x < DISPLAY_WIDTH_PX; x++) {

                    // Create pixel to-scale
                    SDL_Rect px; 
                    px.x = x * SCALE;
                    px.y = y * SCALE;
                    px.w = SCALE;
                    px.h = SCALE;
                    
                    if (chip8.display.bits[y][x] == 0) {
                        SDL_SetRenderDrawColor(chip8.display.renderer, 
                            0, 0, 0, 255); // Black
                    } else { 
                        SDL_SetRenderDrawColor(chip8.display.renderer, 
                            255, 255, 255, 255); // White
                    }
                    SDL_RenderFillRect(chip8.display.renderer, &px); 
                }
            }

            // Update display
            SDL_RenderPresent(chip8.display.renderer);
        }
        
        if (chip8.delay_timer > 0) 
            chip8.delay_timer--;

        if (chip8.sound_timer > 0) {
            Mix_PlayChannel(-1, sound_timer_beep, 0);
            chip8.sound_timer--;
        }

    }

    SDL_Quit();
    return 0;
}