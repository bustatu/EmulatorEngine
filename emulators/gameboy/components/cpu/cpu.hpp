#ifndef GAMEBOY_CPU_H
#define GAMEBOY_CPU_H

#include "../bus/bus.hpp"
#include <cstring>
#include <sstream>

// Amazing CPU implementation, I liked the register handling so I will use that here, too.
// Sorry to hear his head was hurting while making these but I am using them so mine doesn't hurt. (頭いてェー)
// https://github.com/marethyu/noufu/

namespace Gameboy
{
    class CPU
    {
    private:
        // The CPU registers, in order (for low endinan machines):
        // C, B, E, D, L, H, F, A, SP low, SP high
        uint8_t reg8[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

        // What the fuck
        uint16_t *reg16 = (uint16_t *) reg8;

        // Mirror those values (didn't know about this before)
        uint8_t &A = reg8[7], &F = reg8[6], &C = reg8[0];
        uint16_t &HL = reg16[2];
        uint16_t &SP = reg16[4];

        // Order: B, C, D, E, H, L, F, A
        uint8_t *reg8_group[8] = {reg8 + 1, reg8, reg8 + 3, reg8 + 2, reg8 + 5, reg8 + 4, &F, &A};

        // Register group 1 for operations that include the SP:
        // Order: BC, DE, HL, SP
        uint16_t *reg16_group1[4] = {reg16, reg16 + 1, &HL, &SP};

        // Register group 2 for operations that include the HL(-+):
        // Order: BC, DE, HL, HL
        uint16_t *reg16_group2[4] = {reg16, reg16 + 1, &HL, &HL};

        // Register group 2 for the rest of the operations
        // Order: BC, DE, HL, AF
        uint16_t *reg16_group3[4] = {reg16, reg16 + 1, &HL, reg16 + 3};

        // Program counter
        uint16_t PC, oldPC;

        // Instruction main byte
        uint8_t opcode;

        // Execution halting timer
        uint32_t waitTimer;

        // Interrupt management
        bool ime_flag;
        uint8_t ei_delay;
        uint8_t IE, IF;

        // Halting management
        uint8_t halt_mode = 3; // Mode 3 = no halt
        bool halted = false;
        bool haltbug = false, lock = false;

        // Unknown opcode handlers
        void unknown_cb();
        void unknown();

        // Auxiliary functions
        bool get_condition(uint8_t what);
        void push(uint16_t data);
        uint16_t pop();
        void call(uint16_t addr);
        uint8_t get_byte(uint16_t where);
        uint16_t get_word(uint16_t where);
        void write_byte(uint16_t where, uint8_t what);
        void write_word(uint16_t where, uint16_t what);
        void shift_right(uint8_t &n, bool logical);
        void shift_left(uint8_t &n);
        void rotate_right(uint8_t &n, bool circular, bool resetZ);
        void rotate_left(uint8_t &n, bool circular, bool resetZ);

    public:
        // Constructor
        CPU();

        // Bus handler
        Bus* bus = nullptr;

        // Skips the BIOS (use this before you start executing)
        void skip_bios();

        // Requests an interrupt
        void requestInterrupt(uint8_t what);

        // Executes the next instruction
        void execute();
    };
}

#endif