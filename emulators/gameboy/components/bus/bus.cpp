#include "bus.h"

void Gameboy_Bus::attachBIOS(Gameboy_BIOS* newBIOS)
{
    bios = newBIOS;
}

void Gameboy_Bus::attachRAM(Gameboy_RAM* newRAM)
{
    ram = newRAM;
}

uint8_t Gameboy_Bus::readByte(uint16_t addr)
{
    // BIOS area
    if(addr >= 0x0000 && addr <= 0x3FFF)
    {
        return bios -> readByte(addr);
    }

    // Not implemented
    return 0;
}

uint16_t Gameboy_Bus::readWord(uint16_t addr)
{
    // BIOS area
    if(addr >= 0x0000 && addr <= 0x3FFE)
    {
        return bios -> readWord(addr);
    }

    // Not implemented
    return 0;
}

void Gameboy_Bus::writeWord(uint16_t addr, uint16_t val)
{
    ram -> writeWord(addr, val);
    printf("\033[1;31m{E}: Unimplemented write at address %04X, value %04X \033[0m\n", addr, val);
}