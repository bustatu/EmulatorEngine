#ifndef GAMEBOY_MBC_H
#define GAMEBOY_MBC_H

#include <iostream>

namespace Gameboy
{
    class MBC
    {
    public:
        // Init from file contents
        virtual void init(uint8_t* fileContents, uint32_t fileSize) = 0;

        // 0x0000 - 0x3FFF
        virtual uint8_t readByteFromBank00(uint16_t addr) = 0;
        virtual void writeByteToBank00(uint16_t addr, uint8_t what) = 0;

        // 0x4000 - 0x7FFF
        virtual uint8_t readByteFromBankNN(uint16_t addr) = 0;
        virtual void writeByteToBankNN(uint16_t addr, uint8_t what) = 0;

        // 0xA000 - 0xBFFF
        virtual uint8_t readByteFromERAM(uint16_t addr) = 0;
        virtual void writeByteToERAM(uint16_t addr, uint8_t what) = 0;
    };
}


#endif