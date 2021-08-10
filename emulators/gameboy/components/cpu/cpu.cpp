#include "cpu.h"

uint16_t Gameboy_CPU::get_bc()
{
    return (reg_b << 8) | reg_c;
}

void Gameboy_CPU::set_bc(uint16_t val)
{
    reg_b = (val & 0xFF00) >> 8;
    reg_c = val & 0xFF;
}