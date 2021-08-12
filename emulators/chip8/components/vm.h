#ifndef CHIP8_VM_H
#define CHIP8_VM_H

#include "../../../window.h"
#include "input.h"
#include "graphics.h"
#include <SDL2/SDL.h>
#include <cstring>
#include <sstream>
#include <time.h>

namespace CHIP8
{
    class VM
    {
    private:
        // Input handler
        Input input;

        // Graphics handler
        Graphics graphics;

        // 4096 bytes of storage
        uint16_t ram_size = 0x1000;
        
        // Location of start of ROM
        uint16_t rom_start = 0x200;

        // RAM
        uint8_t *ram;

        // Stack, size and pointer
        uint16_t* stack;
        uint16_t stack_size = 16, sp;

        // Clock execution timings and speeds
        uint32_t frequency = 400; // in instructions per second
        double execTimer, deltaTimer;

        // Timers
        uint8_t timers[2];

        // Registers
        uint8_t *V;
        uint16_t PC, I;

        // State
        uint8_t state;

        // Current fectched opcode
        uint16_t opcode;

        // More complex opcodes
        void DRW(uint8_t va, uint8_t vb, uint8_t vc);
        void unknown(uint16_t opcode);

        // User flags
        uint8_t* user_flags;

    public:
        // Constructor
        VM();

        // Destructor
        ~VM();

        // Start VM
        void boot();

        // Update VM
        void update(double dt);

        // Returns state
        uint8_t get_state();

        // Gets current ROM start location
        uint16_t get_rom_start();

        // Sets status of a key
        void set_key(uint8_t who, uint8_t what);

        // Sets current ROM start location
        void set_rom_start(uint16_t offset);

        // Gets current screen size
        std::pair<int, int> getSize();

        // Reads a byte from RAM
        uint8_t read(uint16_t offset);

        // Sets timer (0 - delay, 1 - sound)
        void set_timer(uint8_t who, uint8_t what);

        // Gets timer (0 - delay, 1 - sound)
        uint8_t get_timer(uint8_t who);

        // Stores a byte to RAM
        void write(uint16_t offset, uint8_t value);

        // Stores multiple bytes to RAM
        int8_t writeBuffer(uint16_t offset, uint8_t* buffer, uint32_t size);

        // Sets register to a value
        void set_reg(uint8_t who, uint8_t what);

        // Sets value of register I
        void set_I(uint16_t what);

        // Update keys from the window
        void updateKeys(Window* win);

        // Draws to a texture using a renderer
        void draw(SDL_Texture* &target, SDL_Renderer* tool);

        // Disassemblies one opcode
        std::string disassembly(uint16_t opcode);

        // Executes one opcodeweeb art
        void execute(uint16_t opcode);
    };
}

#endif