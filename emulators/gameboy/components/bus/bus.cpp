#include "bus.h"

void Gameboy_Bus::attachBIOS(Gameboy_BIOS* newBIOS)
{
    bios = newBIOS;
}

void Gameboy_Bus::attachRAM(Gameboy_RAM* newRAM)
{
    ram = newRAM;
}

void Gameboy_Bus::attachGPU(Gameboy_GPU* newGPU)
{
    gpu = newGPU;
}

void Gameboy_Bus::attachROM(Gameboy_ROM* newROM)
{
    rom = newROM;
}

uint8_t Gameboy_Bus::readByte(uint16_t addr)
{
    // BIOS area
    if(addr >= 0x0000 && addr <= 0x3FFF)
    {
        // TODO: ROMs load from this area too.
        return bios -> readByte(addr);
    }
    // ROM area
    else if(addr <= 0x7FFF)
    {
        return rom -> readByte(addr);       
    }
    else
    {
        // RAM and GPU area
        if(addr <= 0x9FFF && addr >= 0x8000)
            return gpu -> readByte(addr - 0x8000);
        else           
            return ram -> readByte(addr);
    }

    // Not implemented
    printf("\033[1;31m{E}: Unimplemented readByte at address %04X\033[0m\n", addr);
    exit(0);
}

uint16_t Gameboy_Bus::readWord(uint16_t addr)
{
    // BIOS area
    if(addr >= 0x0000 && addr <= 0x3FFE)
    {
        return bios -> readWord(addr);
    }
    // ROM area
    else if(addr <= 0x7FFF)
    {
        return rom -> readWord(addr);       
    }
    // RAM area
    else 
    {
        return ram -> readWord(addr);
    }

    // Not implemented
    printf("\033[1;31m{E}: Unimplemented readWord at address %04X\033[0m\n", addr);
    exit(0);
}

void Gameboy_Bus::writeByte(uint16_t addr, uint8_t val)
{
    if(addr > 0x7FFF)
    {
        if(addr <= 0x9FFF && addr >= 0x8000)
            gpu -> writeByte(addr - 0x8000, val);
        else
            ram -> writeByte(addr, val);
    }
    else
        printf("\033[1;31m{E}: Unimplemented writeByte at address %04X, value %04X \033[0m\n", addr, val);
}

void Gameboy_Bus::writeWord(uint16_t addr, uint16_t val)
{
    if(addr > 0x7FFF)
        ram -> writeWord(addr, val);
    else
        printf("\033[1;31m{E}: Unimplemented writeWord at address %04X, value %04X \033[0m\n", addr, val);
}