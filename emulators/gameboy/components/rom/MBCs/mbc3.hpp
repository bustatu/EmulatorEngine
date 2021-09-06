#ifndef GAMEBOY_MBC_3_H
#define GAMEBOY_MBC_3_H

#include "mbc.hpp"
#include <cstring>

namespace Gameboy
{
    class MBC3 : public MBC
    {
    private:
        // ROM data and RAM data
        uint8_t *ROM, *RAM;
        uint8_t rom_size, ram_size;
        uint8_t rom_bank = 0x01;
        bool ram_enabled = false;

    public:
        void init(uint8_t* fileContents, uint32_t fileSize, std::string fileName);

        // 0x0000 - 0x3FFF
        uint8_t readByteFromBank00(uint16_t addr);
        void writeByteToBank00(uint16_t addr, uint8_t what);
        
        // 0x4000 - 0x7FFF
        uint8_t readByteFromBankNN(uint16_t addr);
        void writeByteToBankNN(uint16_t addr, uint8_t what);

        // 0xA000 - 0xBFFF
        uint8_t readByteFromERAM(uint16_t addr);
        void writeByteToERAM(uint16_t addr, uint8_t what);

        // Destructor
        ~MBC3();
    };
}


#endif