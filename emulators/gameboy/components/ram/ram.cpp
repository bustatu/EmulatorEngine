#include "ram.h"

namespace Gameboy
{
    RAM::RAM()
    {
        // Initialise the RAM
        data = new uint8_t[0x10000];
        data[0xFF50] = 0;
    }

    uint16_t RAM::readWord(uint16_t addr)
    {
        return (data[addr + 1] << 8) | data[addr];
    }

    uint8_t RAM::readByte(uint16_t addr)
    {
        return data[addr];
    }

    void RAM::writeByte(uint16_t addr, uint8_t what)
    {
        data[addr] = what;
    }

    void RAM::writeWord(uint16_t addr, uint16_t what)
    {
        data[addr] = what & 0xFF;
        data[addr + 1] = (what & 0xFF00) >> 8;
    }
}
