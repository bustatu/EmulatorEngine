#include "ram.h"

Gameboy_RAM::Gameboy_RAM()
{
    // Initialise the RAM
    data = new uint8_t[0x10000];
}

uint16_t Gameboy_RAM::readWord(uint16_t addr)
{
    return (data[addr + 1] << 8) | data[addr];
}

uint8_t Gameboy_RAM::readByte(uint16_t addr)
{
    return data[addr];
}

void Gameboy_RAM::writeByte(uint16_t addr, uint8_t what)
{
    data[addr] = what;
}

void Gameboy_RAM::writeWord(uint16_t addr, uint16_t what)
{
    data[addr] = what & 0xFF;
    data[addr + 1] = (what & 0xFF00) >> 8;
}