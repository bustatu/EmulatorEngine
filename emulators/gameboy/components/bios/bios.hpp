#ifndef GAMEBOY_BIOS_H
#define GAMEBOY_BIOS_H

#include <fstream>

namespace Gameboy
{
    class BIOS
    {
    private:
        // BIOS data
        uint8_t* data;

        // Flag if the BIOS has been loaded
        bool loaded = false;

    public:
        // Constructor
        BIOS();

        // Load BIOS from the location
        void load(std::string path);

        // Returns loaded state
        bool isLoaded();

        // Reads a byte from the specified offset
        uint8_t readByte(uint16_t addr);
    };
};

#endif