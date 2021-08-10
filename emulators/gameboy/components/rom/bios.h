#ifndef GAMEBOY_BIOS_H
#define GAMEBOY_BIOS_H

#include <fstream>

class Gameboy_BIOS
{
private:
    // BIOS data
    uint8_t* data;

    // Flag if the BIOS has been loaded
    bool loaded = false;

public:
    // Constructor
    Gameboy_BIOS();

    // Load BIOS from the location
    void load(std::string path);
};

#endif