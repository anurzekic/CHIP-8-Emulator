#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include "./include/chip8.h"
#include <assert.h>

bool init_SDL() { //, SDL_Window *window, SDL_Renderer *renderer) {
    if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO) != 0) { 
        SDL_Log("Unable to initialize SDL: %s\n", SDL_GetError());
        return false;
    }

    // window = SDL_CreateWindow("CHIP-8 Emulator", 
    //                         SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
    //                         DEFAULT_WIDTH * config.window_scale_factor, 
    //                         DEFAULT_HEIGHT * config.window_scale_factor, 0); // TODO Add resizable flag later
 
    // if (!window) {
    //     SDL_Log("Unable to create window: %s\n", SDL_GetError());
    //     return false;
    // }

    // renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    
    // if (!renderer) {
    //     SDL_Log("Unable to create renderer: %s\n", SDL_GetError());
    //     return false;
    // }

    return true;
}

void cleanup_SDL(SDL_Window *window, SDL_Renderer *renderer) {
    if (renderer)
        SDL_DestroyRenderer(renderer);

    if (window)
        SDL_DestroyWindow(window);

    SDL_Quit();
}

void clear_window(SDL_Renderer *renderer, configuration_t &config) {
    printf("CONFIG: %u %u %u %u\n", config.red, config.green, config.blue, config.alpha);
    if(renderer == NULL){
        assert("RENDERER NULL " && false);
    }
    SDL_SetRenderDrawColor(renderer, config.red, config.green, config.blue, config.alpha); // Set color to solid white
    SDL_RenderClear(renderer);
}

void update_window(SDL_Renderer *renderer) {
    SDL_RenderPresent(renderer);
}

void init_config(configuration_t &config, uint8_t scale_factor, uint8_t r, uint8_t g, 
                 uint8_t b, uint8_t a) {
    config.window_scale_factor = scale_factor;
    config.red   = r;
    config.green = g;
    config.blue  = b;
    config.alpha = a;
}

int main(int argc, char const *argv[])
{
    (void)argc;
    (void)argv;
    // TODO Add audio somehow
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    
    Chip8 chip8_instance;
    configuration_t config;
    SDL_Event event;

    init_config(config, 10, 0, 0, 255, 255);
    
    if (!init_SDL())
        exit(EXIT_FAILURE);

    window = SDL_CreateWindow("CHIP-8 Emulator", 
                        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                        DEFAULT_WIDTH * config.window_scale_factor, 
                        DEFAULT_HEIGHT * config.window_scale_factor, 0); // TODO Add resizable flag later

    if (!window) {
        SDL_Log("Unable to create window: %s\n", SDL_GetError());
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    
    if (!renderer) {
        SDL_Log("Unable to create renderer: %s\n", SDL_GetError());
        return false;
    }
    
    clear_window(renderer, config);
    // SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 255); // Set color to solid white
    // SDL_RenderClear(renderer);

    while (chip8_instance.state != QUIT) {
        while (SDL_PollEvent(&event)) {
            switch (event.type ) {
            case SDL_QUIT:
                chip8_instance.state = QUIT;
                printf("Quitting!\n");
                break;

            case SDL_KEYDOWN:
                printf("Key press detected\n");
                break;

            case SDL_KEYUP:
                printf("Key release detected\n");
                break;

            default:
                break;
            }

        }
        SDL_Delay(16); // Figure this shit out

        update_window(renderer);
    }

    // SDL_Delay(3000); // For testing purposes
    cleanup_SDL(window, renderer);
    exit(EXIT_SUCCESS);
}
