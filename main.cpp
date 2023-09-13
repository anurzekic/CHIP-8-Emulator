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

void init_chip8(Chip8& instance, const char* filename) {
    memset(instance.RAM, 0, sizeof(Chip8));

    // Load font 
    // memcpy(&instance->ram[0], FONT, sizeof(FONT)); // Put the chip8 instance on the heap
       
    FILE *file;
    file = fopen(filename, "r+b"); // Ne treba write ? 
    fread(&instance.RAM[0x200], sizeof(instance.RAM), 1, file);
    instance.pc = 0x200;

    // for(int i = 0; i < 4096; i++)
    //     printf("%u ", instance.RAM[i]); // prints a series of bytes

}

void decode_instruction(Chip8& instance) {    
    uint16_t opcode = instance.RAM[instance.pc] & 0xF000;
    printf("INSTRUCTION %x\n", opcode);
    // switch () // Most significant byte is stored first
    // {
    // case :
    //     break;
    
    // default:
    //     break;
    // }
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

    init_chip8(chip8_instance, argv[1]);
    decode_instruction(chip8_instance);

    while (chip8_instance.state != QUIT) {
        while (SDL_PollEvent(&event)) {
            const Uint8* state = SDL_GetKeyboardState(NULL);
            
            switch (event.type) {
            case SDL_QUIT:
                chip8_instance.state = QUIT;
                printf("Quitting!\n");
                break;

            case SDL_KEYDOWN:
                if (state[SDL_SCANCODE_ESCAPE]) {
                    chip8_instance.state = QUIT; 
                }
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

        // Emulate instructions here

        update_window(renderer);
    }

    // SDL_Delay(3000); // For testing purposes
    cleanup_SDL(window, renderer);
    exit(EXIT_SUCCESS);
}
