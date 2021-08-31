#ifndef GAMEBOY_MBC_0_H
#define GAMEBOY_MBC_0_H

#include "mbc.hpp"
#include <cstring>

namespace Gameboy
{
    class MBC0 : public MBC
    {
    private:
        // ROM data and RAM data
        uint8_t ROM[0x8000];
        uint8_t RAM[0x2000];

    public:
        void init(uint8_t* fileContents, uint32_t fileSize);

        // 0x0000 - 0x3FFF
        uint8_t readByteFromBank00(uint16_t addr);
        void writeByteToBank00(uint16_t addr, uint8_t what);
        
        // 0x4000 - 0x7FFF
        uint8_t readByteFromBankNN(uint16_t addr);
        void writeByteToBankNN(uint16_t addr, uint8_t what);

        // 0xA000 - 0xBFFF
        uint8_t readByteFromERAM(uint16_t addr);
        void writeByteToERAM(uint16_t addr, uint8_t what);
    };
}


#endif