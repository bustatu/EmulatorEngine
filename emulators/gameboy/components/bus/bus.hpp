#ifndef GAMEBOY_BUS_H
#define GAMEBOY_BUS_H

#include "../bios/bios.hpp"
#include "../ram/ram.hpp"
#include "../rom/rom.hpp"

namespace Gameboy
{
    class Bus
    {
    private:
        BIOS* bios = nullptr;
        RAM* ram = nullptr;
        ROM* rom = nullptr;

    public:
        // Attach the RAM to the bus
        void attachRAM(RAM* newRAM);

        // Attach the ROM to the bus
        void attachROM(ROM* newRAM);

        // Attach a BIOS to the bus
        void attachBIOS(BIOS* newBIOS);

        // Gets a byte from memory
        uint8_t readByte(uint16_t addr);

        // Gets 2 bytes from memory
        uint16_t readWord(uint16_t addr);

        // Writes a byte to memory
        void writeByte(uint16_t addr, uint8_t val);

        // Writes 2 bytes to memory
        void writeWord(uint16_t addr, uint16_t val);
    };
}

#endif