#ifndef CHIP8
#define CHIP8

#include <stdint.h>
#include <stack>

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
    // std::array<uint16_t, 16> stack;
    // uint16_t stack[16];
    std::stack<uint16_t> stack;

    uint16_t opcode;
    
    // Insturctions per second
    uint16_t instr_per_sec;
    
    // Decremented at a rate of 60Hz
    uint8_t  delay_timer; 
    uint8_t  sound_timer;

    // Draw when the flag is set
    bool draw;

    uint16_t keypad[16];
    
    //                Y   X
    uint8_t  display[32][64];

    chip8_state_t state;
  // public:
    Chip8();
    ~Chip8() = default;
};

#endif