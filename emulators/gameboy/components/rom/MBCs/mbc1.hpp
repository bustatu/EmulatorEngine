#ifndef GAMEBOY_MBC_1_H
#define GAMEBOY_MBC_1_H

#include "mbc.hpp"
#include <fstream>
#include <cstring>

namespace Gameboy
{
    class MBC1 : public MBC
    {
    private:
        // ROM data and RAM data
        uint8_t *ROM;
        uint8_t *RAM;
        uint32_t ramByteSize;
        std::string rom_name;
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
        ~MBC1();
    };
}


#endif