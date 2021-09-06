#include "mbc5.hpp"

#define get_bit(who, which) (((who) >> (which)) & 1)
#define set_bit(who, which, what) who = (what) ? ((who) | (1 << (which))) : ((who) & ~(1 << (which)))

namespace Gameboy
{
    void MBC5::init(uint8_t* fileContents, uint32_t fileSize, std::string fileName)
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
                case 0x2:
                    RAM = new uint8_t[8192];
                    break;
                case 0x3:
                    RAM = new uint8_t[32768];
                    break;
                case 0x4:
                    RAM = new uint8_t[131072];
                    break;
            }
        }
    }

    uint8_t MBC5::readByteFromBank00(uint16_t addr)
    {
        if(addr >= 0x0000 && addr <= 0x3FFF)
            return ROM[addr];
        return 0xFF;       
    }

    void MBC5::writeByteToBank00(uint16_t addr, uint8_t what)
    {
        // Enable RAM
        if(addr >= 0x0000 && addr <= 0x1FFF)
            enable_ram = ((what & 0b11111) == 0xA);
        // ROM bank 8 bits
        else if(addr >= 0x2000 && addr <= 0x2FFF)
            rom_bank_number = what & 0x1FF;
        // ROM bank 9th bit
        else if(addr >= 0x3000 && addr <= 0x3FFF)
            rom_bank_number = (what & 1) << 8;
    }

    uint8_t MBC5::readByteFromBankNN(uint16_t addr)
    {
        if(addr >= 0x4000 && addr <= 0x7FFF)
            return ROM[0x4000 * rom_bank_number + (addr - 0x4000)];
        return 0xFF; 
    }

    void MBC5::writeByteToBankNN(uint16_t addr, uint8_t what)
    {
        // RAM Bank
        if(addr >= 0x4000 && addr <= 0x5FFF)
            ram_bank_number = what & 0x0F;
        // Mode select
        else if(addr >= 0x6000 && addr <= 0x7FFF)
            printf("{W}: Write to unmapped adress: %04X, value: %04X\n", addr, what);
    }

    uint8_t MBC5::readByteFromERAM(uint16_t addr)
    {
        if(enable_ram && ram_size)
            return RAM[0x2000 * ram_bank_number + (addr - 0xA000)];
        return 0xFF;
    }

    void MBC5::writeByteToERAM(uint16_t addr, uint8_t what)
    {
        if(enable_ram && ram_size)
            RAM[0x2000 * ram_bank_number + (addr - 0xA000)] = what;
    }

    MBC5::~MBC5()
    {
        
    }
}