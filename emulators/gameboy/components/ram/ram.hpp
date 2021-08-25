#ifndef GAMEBOY_RAM_H
#define GAMEBOY_RAM_H

#include <iostream>

namespace Gameboy
{
    class RAM
    {
    private:
        // RAM data
        uint8_t* data;

    public:
        // Constructor
        RAM();

        // Reads a byte from the specified address
        uint8_t readByte(uint16_t addr);

        // Reads a word from the specified address
        uint16_t readWord(uint16_t addr);

        // Writes a byte to the specified address
        void writeByte(uint16_t addr, uint8_t what);

        // Writes a word to the specified address
        void writeWord(uint16_t addr, uint16_t what);
    };
}

#endif