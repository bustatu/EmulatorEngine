#include "mbc1.hpp"

namespace Gameboy
{
    void MBC1::init(uint8_t* fileContents, uint16_t fileSize)
    {
        // Get ROM size in banks (1 << (rom_size + 1)), same for RAM size
        rom_size = fileContents[0x148];
        ram_size = fileContents[0x149];

        // Allocate ROM and RAM
        ROM = new uint8_t[0x8000 * (1 << rom_size)];
        memcpy(ROM, fileContents, fileSize);
        if(ram_size != 0)
            RAM = new uint8_t[1024 * (1 << (2 * rom_size - 2))];

        // Calculate ROM bank mask
        rom_bank_mask = 0;
        for(int32_t i = 0; i < (rom_size < 5 ? rom_size : 5); i++)
            rom_bank_mask |= (1 << i);
    }

    uint8_t MBC1::readByteFromBank00(uint16_t addr)
    {
        if(addr >= 0x0000 && addr <= 0x3FFF)
        {
            if(mode_flag)
            {

            }
            else
                return ROM[addr];
        }
        return 0xFF;       
    }

    void MBC1::writeByteToBank00(uint16_t addr, uint8_t what)
    {
        // Enable RAM
        if(addr >= 0x0000 && addr <= 0x1FFF)
            enable_ram = (what & 0xF == 0xA);
        // ROM bank
        else if(addr >= 0x2000 && addr <= 0x3FFF)
            if(what)
                rom_bank_number = what & rom_bank_mask;
            else
                rom_bank_number = 1;
    }

    uint8_t MBC1::readByteFromBankNN(uint16_t addr)
    {
        if(addr >= 0x4000 && addr <= 0x7FFF)
            return ROM[addr];
        return 0xFF; 
    }

    void MBC1::writeByteToBankNN(uint16_t addr, uint8_t what)
    {
        // RAM Bank
        if(addr >= 0x4000 && addr <= 0x5FFF)
            ram_bank_number = what & 0b11;
        // Mode select
        else if(addr >= 0x6000 && addr <= 0x7FFF)
            mode_flag = what & 0b1;
    }

    uint8_t MBC1::readByteFromERAM(uint16_t addr)
    {
        addr -= 0xA000;
        if(addr >= 0x0000 && addr <= 0x1FFF)
            return RAM[addr];
        return 0xFF;
    }

    void MBC1::writeByteToERAM(uint16_t addr, uint8_t what)
    {
        if(enable_ram && ram_size)
        {
            // If 32 kb and mode flag 1
            if(ram_size == 0x03 && mode_flag)
                addr = 0x2000 * ram_bank_number + (addr - 0xA000);
            // If 32 kb
            else if(ram_size == 0x03)
                addr -= 0xA000;
            else
                addr = (addr - 0xA000) % ((ram_size == 0x01) ? 2048 : 8192);
        }
    }
}