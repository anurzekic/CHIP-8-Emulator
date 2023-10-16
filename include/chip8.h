#ifndef CHIP8
#define CHIP8

#include <stdint.h>
#include <stack>
#include <SDL2/SDL.h>

#define DISPLAY_WIDTH  64 // CHIP-8 default width
#define DISPLAY_HEIGHT 32 // CHIP-8 default width

const static uint8_t FONT[80] = {
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
  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

// const int key_mappings[16] = {
//   SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3, SDL_SCANCODE_4,
//   SDL_SCANCODE_Q, SDL_SCANCODE_W, SDL_SCANCODE_E, SDL_SCANCODE_R,
//   SDL_SCANCODE_A, SDL_SCANCODE_S, SDL_SCANCODE_D, SDL_SCANCODE_F,
//   SDL_SCANCODE_Z, SDL_SCANCODE_X, SDL_SCANCODE_C, SDL_SCANCODE_V 
// };

const int key_mappings[16] = {
  //     0               1              2               3
  SDL_SCANCODE_X, SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3,
  //     4               5              6               7
  SDL_SCANCODE_Q, SDL_SCANCODE_W, SDL_SCANCODE_E, SDL_SCANCODE_A, 
  //     8               9              A               B  
  SDL_SCANCODE_S, SDL_SCANCODE_D, SDL_SCANCODE_Z, SDL_SCANCODE_C,
  //     C               D              E               F  
  SDL_SCANCODE_4, SDL_SCANCODE_R, SDL_SCANCODE_F, SDL_SCANCODE_V 
};

typedef enum 
{
  RUNNING,
  PAUSED,
  QUIT  
} chip8_state_t;

typedef struct 
{
  uint8_t window_scale_factor;
  uint8_t red;
  uint8_t blue;
  uint8_t green;
  uint8_t alpha;
} configuration_t;

class Chip8
{
  // private:
  public:
    uint8_t  RAM[4096];

    uint8_t  V[16]; // General purpose variable registers V0 to VF 
    uint16_t I;     // Index register
    
    uint8_t  sp;    // Stack pointer
    uint16_t pc;    // Program counter
    
    std::stack<uint16_t> stack;

    uint16_t opcode;
    
    // Insturctions per second
    uint16_t instr_per_sec;
    
    // Decremented at a rate of 60Hz
    uint8_t  delay_timer; 
    uint8_t  sound_timer;

    // Draw when the flag is set
    bool draw;

    uint8_t keypad[16];
    
    //                Y   X
    uint8_t  display[32][64];

    chip8_state_t state;

    Chip8();
    ~Chip8() = default;
};

#endif