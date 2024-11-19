#include <SDL.h>
#include <stdbool.h>

int main() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

    SDL_CreateWindow("Chip-8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 64 * 16, 32 * 16, 0);

    while (true) {
        SDL_Event event;
        
        while (SDL_PollEvent(&event)) {
            switch (event.type) {

                case SDL_KEYDOWN:
                    if (event.key.keysym.scancode == 20) 
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