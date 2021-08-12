#ifndef GAMEBOY_BUS_H
#define GAMEBOY_BUS_H

#include "../rom/rom.h"
#include "../rom/bios.h"
#include "../ram/ram.h"

class Gameboy_Bus
{
private:
    Gameboy_BIOS* bios = nullptr;
    Gameboy_RAM* ram = nullptr;
    Gameboy_ROM* rom = nullptr;

public:
    // Attach the RAM to the bus
    void attachRAM(Gameboy_RAM* newRAM);

    // Attach the ROM to the bus
    void attachROM(Gameboy_ROM* newRAM);

    // Attach a BIOS to the bus
    void attachBIOS(Gameboy_BIOS* newBIOS);

    // Gets a byte from memory
    uint8_t readByte(uint16_t addr);

    // Gets 2 bytes from memory
    uint16_t readWord(uint16_t addr);

    // Writes a byte to memory
    void writeByte(uint16_t addr, uint8_t val);

    // Writes 2 bytes to memory
    void writeWord(uint16_t addr, uint16_t val);
};

#endif