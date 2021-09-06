#include "mbc3.hpp"

#define get_bit(who, which) (((who) >> (which)) & 1)
#define set_bit(who, which, what) who = (what) ? ((who) | (1 << (which))) : ((who) & ~(1 << (which)))

namespace Gameboy
{
    void MBC3::init(uint8_t* fileContents, uint32_t fileSize, std::string fileName)
    {
        // Get ROM size in banks, same for RAM size
        rom_size = fileContents[0x148];
        ram_size = fileContents[0x149];

        // Allocate ROM and RAM
        ROM = new uint8_t[fileSize];
        memcpy(ROM, fileContents, fileSize);

        if(ram_size != 0)
        {
            switch(ram_size)
            {
                case 0x1:
                    RAM = new uint8_t[2048];
                    break;
                case 0x2:
                    RAM = new uint8_t[8192];
                    break;
                case 0x3:
                    RAM = new uint8_t[32768];
                    break;
            }
        }
    }

    uint8_t MBC3::readByteFromBank00(uint16_t addr)
    {
        if(addr >= 0x0000 && addr <= 0x3FFF)
            return ROM[addr];
        return 0xFF;
    }

    void MBC3::writeByteToBank00(uint16_t addr, uint8_t what)
    {
        if(addr >= 0x2000 && addr <= 0x3FFF)
        {
            if(what == 0x00)
                rom_bank = 0x01;
            else
                rom_bank = what & 0b1111111;
        }
    }

    uint8_t MBC3::readByteFromBankNN(uint16_t addr)
    {
        if(addr >= 0x4000 && addr <= 0x7FFF)
            return ROM[0x4000 * (rom_bank - 1) + addr];
        return 0xFF;
    }

    void MBC3::writeByteToBankNN(uint16_t addr, uint8_t what)
    {

    }

    uint8_t MBC3::readByteFromERAM(uint16_t addr)
    {
        return 0xFF;       
    }

    void MBC3::writeByteToERAM(uint16_t addr, uint8_t what)
    {

    }

    MBC3::~MBC3()
    {
        
    }
}