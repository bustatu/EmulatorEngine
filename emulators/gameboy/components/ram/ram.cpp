#include "ram.hpp"

namespace Gameboy
{
    RAM::RAM()
    {
        // Initialise the RAM
        data = new uint8_t[0x10000];
        data[0xFF50] = 0;
    }

    uint8_t RAM::readByte(uint16_t addr)
    {
        // Unusable RAM location
        if(addr >= 0xFEA0 && addr <= 0xFEFF)
            return 0xFF;
        return data[addr];
    }

    void RAM::writeByte(uint16_t addr, uint8_t what)
    {
        data[addr] = what;
    }
}
