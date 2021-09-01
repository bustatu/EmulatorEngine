#ifndef GAMEBOY_BUS_H
#define GAMEBOY_BUS_H

#include "../bios/bios.hpp"
#include "../ram/ram.hpp"
#include "../rom/rom.hpp"
#include "../timer/timer.hpp"
#include "../joypad/joypad.hpp"

namespace Gameboy
{
    class Bus
    {
    public:
        // Bus components
        BIOS* bios = nullptr;
        RAM* ram = nullptr;
        ROM* rom = nullptr;
        Joypad* joypad = nullptr;
        Timer* timer = nullptr;

        // Gets a byte from memory
        uint8_t readByte(uint16_t addr);

        // Writes a byte to memory
        void writeByte(uint16_t addr, uint8_t val);

        // Writes 2 bytes to memory
        void writeWord(uint16_t addr, uint16_t val);
    };
}

#endif