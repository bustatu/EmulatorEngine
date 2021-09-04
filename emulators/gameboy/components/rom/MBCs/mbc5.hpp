#ifndef GAMEBOY_MBC_5_H
#define GAMEBOY_MBC_5_H

#include "mbc.hpp"
#include <cstring>

namespace Gameboy
{
    class MBC5 : public MBC
    {
    private:
        // ROM data and RAM data
        uint8_t *ROM;
        uint8_t *RAM;
        bool enable_ram = false;
        bool mode_flag = false;
        uint8_t rom_size = 0;
        uint8_t ram_size = 0;
        uint8_t rom_bank_mask = 0;
        uint8_t rom_bank_number = 1;
        uint8_t ram_bank_number = 0;

        // Auxiliary functions
        uint8_t zerobank();
        uint8_t highbank();

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