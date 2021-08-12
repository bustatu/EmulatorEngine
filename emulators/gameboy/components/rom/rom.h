#ifndef GAMEBOY_ROM_H
#define GAMEBOY_ROM_H

#include <fstream>

// TODO: IMPLEMENT MORE MBCs

class Gameboy_ROM
{
private:
    // ROM data
    uint8_t* data;

public:
    // Load BIOS from the location
    void load(std::string path);

    // Reads a byte from the specified offset
    uint8_t readByte(uint16_t addr);

    // Read a word from the specified offset
    uint16_t readWord(uint16_t addr);
};

#endif