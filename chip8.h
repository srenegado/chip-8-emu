#ifndef _CHIP8_H_
#define _CHIP8_H_

#include <SDL.h>
#include <stdlib.h>
#include <stdbool.h>

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
void process_user_keyboard_input(Chip8* chip8);


/**
 * Emulate a CPU instruction cycle
 */
void fetch_decode_execute(Chip8* chip8);


#endif