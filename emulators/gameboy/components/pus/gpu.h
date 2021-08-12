#ifndef GAMEBOY_GPU_H
#define GAMEBOY_GPU_H

#include <iostream>

class Gameboy_GPU
{
private:
    // RAW VRAM
    uint8_t *vram;

    // Tiles in a more accessible format
    uint8_t tile[8][8][8];
public:
    // Constructor
    Gameboy_GPU();

    // Reads a byte from the VRAM
    uint8_t readByte(uint16_t addr);

    // Writes a byte to the VRAM
    void writeByte(uint16_t addr, uint8_t val);
};

#endif