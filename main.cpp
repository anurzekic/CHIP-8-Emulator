#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include "./include/chip8.h"
#include <assert.h>
#include <cstdlib>

bool init_SDL() { //, SDL_Window *window, SDL_Renderer *renderer) {
    if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO) != 0) { 
        SDL_Log("Unable to initialize SDL: %s\n", SDL_GetError());
        return false;
    }

    // window = SDL_CreateWindow("CHIP-8 Emulator", 
    //                         SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
    //                         DISPLAY_WIDTH * config.window_scale_factor, 
    //                         DISPLAY_HEIGHT * config.window_scale_factor, 0); // TODO Add resizable flag later
 
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
    // Zero out the RAM
    memset(instance.RAM, 0, sizeof(instance.RAM));

    instance.sound_timer = 0;
    instance.delay_timer = 0;

    instance.draw = false;

    memset(instance.keypad, 0, sizeof(instance.keypad));

    // Set all registers to null
    memset(instance.V, 0, sizeof(instance.V));

    // Load font 
    memcpy(&instance.RAM[0], FONT, sizeof(FONT)); // Put the chip8 font in memory
       
    // Set the screen to 0
    memset(&instance.display, 0, sizeof(instance.display));

    FILE *file;
    file = fopen(filename, "r+b"); // Ne treba write ? 
    
    if (!file) {
        printf("Error: Invalid file name!\n");
        exit(-1);
    }

    fread(&instance.RAM[0x200], sizeof(instance.RAM), 1, file);
    instance.pc = 0x200;

    memset(instance.V, 0, sizeof(instance.V));

    printf("PC: %d ", instance.pc);

    for(int i = 1; i < 4096; i+=2)
        printf("%X", instance.RAM[i - 1] << 8 | instance.RAM[i]); // prints a series of bytes
    printf("\n\n");
}

uint8_t get_VX(uint16_t opcode) {
    return (opcode & 0x0F00) >> 8;
}

uint8_t get_VY(uint16_t opcode) {
    return (opcode & 0x00F0) >> 4;
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
                instance.draw = true;
            }
            else if (instance.opcode == 0x00EE) {
                instance.pc = instance.stack.top();
                instance.stack.pop();
            }
            break;
        
        case 0x1:
            instance.pc = 0x0FFF & instance.opcode;
            // printf("OPCODES: %x \n", (instance.opcode & 0xF000) >> 12);
            // printf("Changed PC to: %d - %x\n\n", instance.pc, instance.pc);
            break;
        
        case 0x2:
            instance.stack.push(instance.pc);
            instance.pc = 0x0FFF & instance.opcode;
            break;
        
        case 0x3:
        {
            uint8_t register_index = (instance.opcode & 0x0F00) >> 8;
            uint8_t nn_value = instance.opcode & 0x00FF;
            if (instance.V[register_index] == nn_value) {
                instance.pc += 2;
            }
        }
            break;
        
        case 0x4:
        {
            uint8_t register_index = (instance.opcode & 0x0F00) >> 8;
            uint8_t nn_value = instance.opcode & 0x00FF;
            if (instance.V[register_index] != nn_value) {
                instance.pc += 2;
            }
        }
            break;
        
        case 0x5:
        {
            uint8_t register_index_first = (instance.opcode & 0x0F00) >> 8;
            uint8_t register_index_second = (instance.opcode & 0x00F0) >> 4;
            if (instance.V[register_index_first] == instance.V[register_index_second]) {
                instance.pc += 2;
            }
        }
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
            switch(instance.opcode & 0x000F)
            {
                case 0x0:
                {
                    // VX is set to the value of VY
                    instance.V[get_VX(instance.opcode)] = instance.V[get_VY(instance.opcode)];
                }
                    break;

                case 0x1:
                {
                    uint8_t VX = get_VX(instance.opcode);
                    uint8_t VY = get_VY(instance.opcode);
                    instance.V[VX] = instance.V[VX] | instance.V[VY];
                }
                    break;

                case 0x2:
                {    
                    uint8_t VX = get_VX(instance.opcode);
                    uint8_t VY = get_VY(instance.opcode);
                    instance.V[VX] = instance.V[VX] & instance.V[VY];
                }
                    break;

                case 0x3:
                {
                    uint8_t VX = get_VX(instance.opcode);
                    uint8_t VY = get_VY(instance.opcode);
                    instance.V[VX] = instance.V[VX] ^ instance.V[VY];
                }
                    break;
                
                case 0x4:
                {
                    uint8_t VX = get_VX(instance.opcode);
                    uint8_t VY = get_VY(instance.opcode);

                    if(((uint16_t)instance.V[VX] + (uint16_t)instance.V[VY]) > 255) {
                        instance.V[0xF] = 0x1;
                    }
                    else {
                        instance.V[0xF] = 0x0;
                    }

                    instance.V[VX] = instance.V[VX] + instance.V[VY];
                }
                    break;

                case 0x5:
                {
                    uint8_t VX = get_VX(instance.opcode);
                    uint8_t VY = get_VY(instance.opcode);
                    if (instance.V[VX] > instance.V[VY]) {
                        instance.V[0xF] = 0x1;
                    }
                    else {
                        instance.V[0xF] = 0x0;
                    }
                    instance.V[VX] = instance.V[VX] - instance.V[VY];
                }
                    break;

                case 0x6:
                {
                    uint8_t VX = get_VX(instance.opcode);
                    instance.V[0xF] = instance.V[VX] & 0x1;
                    instance.V[VX] = instance.V[VX] >> 1;
                }
                    break;

                case 0x7:
                {
                    uint8_t VX = get_VX(instance.opcode);
                    uint8_t VY = get_VY(instance.opcode);
                    
                    if (instance.V[VY] > instance.V[VX]) {
                        instance.V[0xF] = 0x1;
                    }
                    else {
                        instance.V[0xF] = 0x0;
                    }
                    instance.V[VX] = instance.V[VY] - instance.V[VX];
                }
                    break;

                case 0xE:
                {
                    uint8_t VX = get_VX(instance.opcode);
                    instance.V[0xF] = (instance.V[VX] & 0x8000) >> 15;
                    instance.V[VX] = instance.V[VX] << 1;
                }
                    break;   

                default:
                    break;            
            }
            break;
        
        case 0x9:
        {
            uint8_t register_index_first = (instance.opcode & 0x0F00) >> 8;
            uint8_t register_index_second = (instance.opcode & 0x00F0) >> 4;
            if (instance.V[register_index_first] != instance.V[register_index_second]) {
                instance.pc += 2;
            }
        }
            break;  

        case 0xA:
            printf("Setting the index register I to value: %x\n", instance.opcode & 0x0FFF);
            instance.I = instance.opcode & 0x0FFF;
            printf("Set the index register I to value: %x in decimal: %d\n\n", instance.I, instance.I);
            break;

        case 0xB:
            // TODO Make configurable
            instance.pc = (instance.opcode & 0x0FFF) + instance.V[0x0];
            break;  
        
        case 0xC:
            instance.V[get_VX(instance.opcode)] = (rand() % 256) & (instance.opcode & 0x00FF);
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
                // printf("\nNext row!\n");
                if (y == DISPLAY_HEIGHT)
                    break;

                // printf("Value at I in RAM is: %x and decimal %d\n\n", instance.RAM[i], instance.RAM[i]);
                uint8_t x_for_current_row = x;

                for(int shift_by = 8; shift_by > 0; shift_by--) {
                    if (x_for_current_row == DISPLAY_WIDTH) 
                        break;

                    bool x_bit_value = (instance.RAM[i] >> (shift_by - 1)) & 0x1;
                    // printf("BIT %d is set to %d\n", shift_by, x_bit_value);
                    // printf("X: %u, Y: %u\n", x_for_current_row, y);

                    if (x_bit_value && instance.display[y][x_for_current_row]) {
                        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                        SDL_RenderDrawPoint(renderer, x_for_current_row, y);
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
            instance.draw = true;
        }
            break;

        case 0xE:
            switch(instance.opcode & 0xFF)
            {
                case 0x9E:
                    if (instance.keypad[get_VX(instance.opcode)])
                        instance.pc += 2;
                    break;

                case 0xA1:
                    if (!instance.keypad[get_VX(instance.opcode)])
                        instance.pc += 2;
                    break;

                default:
                    break;                
            }
            break;  

        case 0xF:
            switch (instance.opcode & 0x00FF) 
            {
                case 0x07:
                    instance.V[get_VX(instance.opcode)] = instance.delay_timer;
                    break;

                // TODO Implement "Get key" instruction
                case 0x0A:

                    break;

                case 0x15:
                    instance.delay_timer = instance.V[get_VX(instance.opcode)];
                    break;

                case 0x18:
                    instance.sound_timer = instance.V[get_VX(instance.opcode)];
                    break;

                case 0x1E:
                    // TODO Add overflow check
                    instance.I += instance.V[get_VX(instance.opcode)];
                    break;

                case 0x29:
                {
                    // Multiply by 5 since every character is 5 bytes long
                    uint8_t font_index = instance.V[get_VX(instance.opcode)] * 5;
                    instance.I = instance.RAM[font_index];
                    // printf("FONT INDEX: %u AND I: %x\n", font_index, instance.I);
                }
                    break;

                case 0x33:
                {
                    uint8_t value = instance.V[get_VX(instance.opcode)];
                    printf("Decimal conversion instruction HEX: %x | DECIMAL: %u | \n", value, value);
                    instance.RAM[instance.I] = (value / 100) % 10;
                    instance.RAM[instance.I + 1] = (value / 10) % 10;
                    instance.RAM[instance.I + 2] = value % 10;
                    printf("F: %u | S: %u | T: %u\n", instance.RAM[instance.I], instance.RAM[instance.I + 1], instance.RAM[instance.I + 2]);
                }
                    break;

                case 0x55:
                    // TODO Implement other behaviour for older CHIP8 games
                    for(uint8_t i = 0x0; i <= get_VX(instance.opcode); i++) {
                        instance.RAM[instance.I + i] = instance.V[i]; 
                    }
                    break;
                    
                case 0x65:
                    // TODO Implement other behaviour for older CHIP8 games
                    for(uint8_t i = 0x0; i <= get_VX(instance.opcode); i++) {
                        instance.V[i] = instance.RAM[instance.I + i]; 
                    }
                    break;
                
                default:
                    break;
                // TODO Imeplement Super Chip-48 Instructions
            }
            break;  

        default:
            break;
    }
}

// void decode_instruction(Chip8& instance) {
//     (void*)instance;
// }

void handle_input(Chip8& instance, SDL_Event& event) {
    while (SDL_PollEvent(&event)) {
        const Uint8* state = SDL_GetKeyboardState(NULL);
        
        switch (event.type) 
        {
            case SDL_QUIT:
                instance.state = QUIT;
                printf("Quitting!\n");
                break;

            case SDL_KEYDOWN:
                if (state[SDL_SCANCODE_ESCAPE]) {
                    instance.state = QUIT; 
                }
                else if (state[SDL_SCANCODE_SPACE]) {
                    if (instance.state == PAUSED) {
                        instance.state = RUNNING;
                    } 
                    else {
                        instance.state = PAUSED;
                    }
                }
                // printf("Key press detected\n");
                break;

            case SDL_KEYUP:
                // printf("Key relesase detected\n");
                break;

            default:
                break;
        }
    }

}

void update_timers(Chip8& instance) {
    if (instance.delay_timer > 0)
        instance.delay_timer--;
    
    if (instance.sound_timer > 0)
        instance.sound_timer--;
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

    // Adjust scale factor later (change 1 to 10)
    init_config(config, 10, 0, 0, 0, 255);
    if (!init_SDL())
        exit(EXIT_FAILURE);

    window = SDL_CreateWindow("CHIP-8 Emulator", 
                        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                        DISPLAY_WIDTH * config.window_scale_factor, 
                        DISPLAY_HEIGHT * config.window_scale_factor, 0); // TODO Add resizable flag later

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
    chip8_instance.instr_per_sec = 700;

    // decode_instruction(chip8_instance);
    // int i = 0;
    while (chip8_instance.state != QUIT) {
        handle_input(chip8_instance, event);

        if (chip8_instance.state == PAUSED)
            continue;

        uint64_t start_frame = SDL_GetPerformanceCounter();

        // Fetch instructions here for one frame 60Hz
        for (uint32_t i = 0; i < chip8_instance.instr_per_sec / 60; i++) {
            fetch_instruction(chip8_instance, renderer, window, config);
        }

        uint64_t end_frame = SDL_GetPerformanceCounter();

        // Time elapsed in ms
        double time_elapsed = ((double)(end_frame - start_frame) * 1000) / SDL_GetPerformanceFrequency();

        SDL_Delay(time_elapsed > 16.666f ? 0 : 16.666f - time_elapsed);

        if (chip8_instance.draw) {
            update_window(renderer);
            chip8_instance.draw = false;
        }

        update_timers(chip8_instance);
    }

    // SDL_Delay(3000); // For testing purposes
    cleanup_SDL(window, renderer);
    exit(EXIT_SUCCESS);
}
