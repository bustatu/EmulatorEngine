#ifndef GAMEBOY_CPU_H
#define GAMEBOY_CPU_H

#include <iostream>

class Gameboy_CPU
{
private:
    // The CPU registers
    uint8_t reg_a, reg_b, reg_c, reg_d, reg_e, reg_f, reg_h, reg_l;
public:
    // Register handling
    uint16_t get_bc();
    void set_bc(uint16_t val);
};

#endif