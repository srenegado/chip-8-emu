#include <SDL.h>
#include <SDL_mixer.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>

#include "chip8.h"


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

        process_user_keyboard_input(&chip8); 

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