#ifndef GAMEBOY_ROM_H
#define GAMEBOY_ROM_H

#include <fstream>
#include "MBCs/mbc.hpp"
#include "MBCs/mbc0.hpp"
#include "MBCs/mbc1.hpp"

namespace Gameboy
{
    class ROM
    {
    private:
        // ROM data
        MBC *data = nullptr;

        // Check if file is actually loaded
        bool loaded = false;

    public:
        // Load BIOS from the location
        void load(std::string path);

        // I/O operations
        uint8_t readByte(uint16_t addr);
        void writeByte(uint16_t addr, uint8_t val);
    };
}

#endif