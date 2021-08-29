#ifndef GAMEBOY_ROM_H
#define GAMEBOY_ROM_H

#include <fstream>

// TODO: IMPLEMENT MORE MBCs

namespace Gameboy
{
    class ROM
    {
    private:
        // ROM data
        uint8_t* data = nullptr;

        // Check if file is actually loaed
        bool loaded = false;

    public:
        // Load BIOS from the location
        void load(std::string path);

        // Reads a byte from the specified offset
        uint8_t readByte(uint16_t addr);

        // Read a word from the specified offset
        uint16_t readWord(uint16_t addr);
    };
}

#endif