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
    memset(instance.RAM, 0, sizeof(instance.RAM)); // Da li treba ovdje Chip8 ili instance.RAM

    // Load font 
    memcpy(&instance.RAM[0], FONT, sizeof(FONT)); // Put the chip8 font in memory
       
    // Set the screen to 0
    memset(&instance.display, 0, sizeof(instance.display));

    FILE *file;
    file = fopen(filename, "r+b"); // Ne treba write ? 
    fread(&instance.RAM[0x200], sizeof(instance.RAM), 1, file);
    instance.pc = 0x200;

    memset(instance.V, 0, sizeof(instance.V));

    printf("PC: %d ", instance.pc);

    for(int i = 1; i < 4096; i+=2)
        printf("%X", instance.RAM[i - 1] << 8 | instance.RAM[i]); // prints a series of bytes
    printf("\n\n");
}



void fetch_instruction(Chip8& instance, SDL_Renderer *renderer, [[maybe_unused]]SDL_Window *window, configuration_t &config) {    
    instance.opcode = instance.RAM[instance.pc] << 8 | instance.RAM[instance.pc + 1]; // & 0xF000;
    instance.pc += 2;
     
    // printf("PC: %d ", instance.pc);
    // printf("INSTRUCTION %x\n", instance.opcode);
    // printf("OPCODES: %x \n", (instance.opcode & 0xF000) >> 12);
    switch ((instance.opcode & 0xF000) >> 12) // Most significant byte is stored first
    {
    case 0x0:
        if (instance.opcode == 0x00E0) {
            printf("Clear screen!\n\n");
            clear_window(renderer, config);
        }
        else if (instance.opcode == 0x00EE) {
            printf("B\n\n");
        }
        break;
    
    case 0x1:
        // printf("AAA\n");
        instance.pc = 0x0FFF & instance.opcode;
        // printf("OPCODES: %x \n", (instance.opcode & 0xF000) >> 12);
        // printf("Changed PC to: %d - %x\n\n", instance.pc, instance.pc);
        break;
    
    case 0x2:
        break;
    
    case 0x3:
        break;
    
    case 0x4:
        break;
    
    case 0x5:
        break;
    
    case 0x6:
    {
        uint8_t register_index = (instance.opcode & 0x0F00) >> 8;
        uint8_t value = instance.opcode & 0x00FF; 
        instance.V[register_index] = value;
        printf("Setting register: %x to %x\n", register_index, value);
    }
        // printf("Setting register: %x to %x\n", (instance.opcode & 0x0F00) >> 8, instance.opcode & 0x00FF);
        printf("The current value at register: %x is: %x\n\n", (instance.opcode & 0x0F00) >> 8, instance.V[(instance.opcode & 0x0F00) >> 8]);
        
        break;
    
    case 0x7:
    {
        uint8_t register_index = (instance.opcode & 0x0F00) >> 8;
        uint8_t value = instance.opcode & 0x00FF; 
        instance.V[register_index] += value;
        printf("Adding value: %x to register: %x\n", value, register_index);
    }
        printf("The current value at register: %x is: %x\n\n", (instance.opcode & 0x0F00) >> 8, instance.V[(instance.opcode & 0x0F00) >> 8]);
        break;
    
    case 0x8:
        break;
    
    case 0x9:
        break;  

    case 0xA:
        printf("Setting the index register I to value: %x\n", instance.opcode & 0x0FFF);
        instance.I = instance.opcode & 0x0FFF;
        printf("Set the index register I to value: %x in decimal: %d\n\n", instance.I, instance.I);
        break;

    case 0xB:
        break;  
    
    case 0xC:
        break;  

    case 0xD:
    {
        uint8_t x = instance.V[(instance.opcode & 0x0F00) >> 8] % 64; // The scaling factor should be used here in some way
        uint8_t y = instance.V[(instance.opcode & 0x00F0) >> 4] % 32; // Here as well

        instance.V[0xF] = 0;

        printf("Drawing starting from x: %d and y: %d\n", x, y);
        
        uint8_t sprite_height = instance.opcode & 0x000F;
        
        printf("Sprite height is: %d and hex: %x\n", sprite_height, sprite_height);
        printf("Index I is: %x and in decimal: %d\n\n", instance.I, instance.I);

        for(size_t i = instance.I; i < instance.I + sprite_height; i++) {
            printf("\nNext row!\n");
            printf("Value at I in RAM is: %x and decimal %d\n\n", instance.RAM[i], instance.RAM[i]);
            
            uint8_t x_for_current_row = x;

            for(int shift_by = 8; shift_by > 0; shift_by--) {
                bool x_bit_value = (instance.RAM[i] >> (shift_by - 1)) & 0x1;
                printf("BIT %d is set to %d\n", shift_by, x_bit_value);
                printf("X: %u, Y: %u\n", x_for_current_row, y);

                if (x_bit_value && instance.display[y][x_for_current_row]) {
                    instance.display[y][x_for_current_row] = 0;
                    instance.V[0xF] = 1;
                }
                else if(x_bit_value && !instance.display[y][x_for_current_row]) {
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                    SDL_RenderDrawPoint(renderer, x_for_current_row, y);
                    instance.display[y][x_for_current_row] = 1;
                }

                // Should stop if the right edge of the screen has been reached
                x_for_current_row++;
            }
            // Should stop if bottom edge of the screen has been reached
            y++;
        }
    }
        break;

    case 0xE:
        break;  

    case 0xF:
        break;  

    default:
        break;
    }
}

// void decode_instruction(Chip8& instance) {
//     (void*)instance;
// }

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

    // Adjust scale factor later (change 1 to 10)
    init_config(config, 10, 0, 0, 0, 255);
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

    // Figure out how to use this
    SDL_RenderSetScale(renderer, 10, 10); // Sets the scaling of the pixels
    
    clear_window(renderer, config);

    init_chip8(chip8_instance, argv[1]);
    // decode_instruction(chip8_instance);
    // int i = 0;
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
                // printf("Key press detected\n");
                // SDL_SetRenderDrawColor( renderer, 100, 255, 255, 255 );
                // SDL_RenderDrawPoint( renderer, i, 0);
                // i++;
                break;

            case SDL_KEYUP:
                // printf("Key release detected\n");
                break;

            default:
                break;
            }

        }
        SDL_Delay(16); // Figure out how the instructions should be delayed

        // Fetch instructions here
        fetch_instruction(chip8_instance, renderer, window, config);

        update_window(renderer);
    }

    // SDL_Delay(3000); // For testing purposes
    cleanup_SDL(window, renderer);
    exit(EXIT_SUCCESS);
}
