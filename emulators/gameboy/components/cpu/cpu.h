#ifndef GAMEBOY_CPU_H
#define GAMEBOY_CPU_H

#include "../bus/bus.h"
#include <sstream>

class Gameboy_CPU
{
private:
    // The CPU registers
    uint16_t PC;
    uint16_t SP;
    uint8_t reg_a, reg_b, reg_c, reg_d, reg_e, reg_f, reg_h, reg_l;

    // Flow handlers
    uint8_t instr_byte;
    bool can_execute = false;

    // Set clock frequency (4194304 Hz)
    uint32_t freq = 4194304;

    // CPU timers
    uint32_t waitTimer;
    double executionTimer;

    // Bus handler
    Gameboy_Bus* bus;

    // Opcode handlers
    void op_xor(uint8_t &a, uint8_t b);
    void unknown();

    // Sets a flag to a value (7 - zero, 6 - sub, 5 - half carry, 4 - carry)
    void set_flag(uint8_t who, uint8_t what);
    // Sets a flag to a value (7 - zero, 6 - sub, 5 - half carry, 4 - carry)
    uint8_t get_flag(uint8_t who);

public:
    // Constructor
    Gameboy_CPU();

    // Update
    void update(double dt);

    // Executes the next instruction
    void execute();

    // Dissasemblies an opcode in the current context of the VM
    std::string dissasembly(uint8_t instr_byte);

    // Attaches the bus to the CPU
    void attachBus(Gameboy_Bus* newBus);

    // Register handling
    uint16_t get_af();
    void set_af(uint16_t val);

    uint16_t get_bc();
    void set_bc(uint16_t val);

    uint16_t get_de();
    void set_de(uint16_t val);

    uint16_t get_hl();
    void set_hl(uint16_t val);
};

#endif