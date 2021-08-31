#include "mbc1.hpp"

namespace Gameboy
{
    static const uint8_t bit_mask[] = {
        0b00000001,
        0b00000011,
        0b00000111,
        0b00001111,
        0b00011111,
        0b00011111,
        0b00011111
    };

    uint8_t MBC1::get_bit(uint8_t who, uint8_t which)
    {
        return ((who >> which) & 1);
    }

    void MBC1::set_bit(uint8_t &who, uint8_t which, uint8_t what)
    {
        if(what)
            who |= (1 << which);
        else
            who &= ~(1 << which);
    }

    uint8_t MBC1::zerobank()
    {
        uint8_t result = 0;

        if(rom_size == 5 && get_bit(ram_bank_number, 0))
            set_bit(result, 5, 1);

        if(rom_size == 6 && get_bit(ram_bank_number, 1))
            set_bit(result, 6, 1);

        return result;
    }

    uint8_t MBC1::highbank()
    {
        uint8_t result = rom_bank_number;

        if(rom_size == 5 && get_bit(ram_bank_number, 0))
            set_bit(result, 5, 1);

        if(rom_size == 6 && get_bit(ram_bank_number, 1))
            set_bit(result, 6, 1);

        return result;
    }

    void MBC1::init(uint8_t* fileContents, uint32_t fileSize)
    {
        // Get ROM size in banks (1 << (rom_size + 1)), same for RAM size
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

    uint8_t MBC1::readByteFromBank00(uint16_t addr)
    {
        if(addr >= 0x0000 && addr <= 0x3FFF)
        {
            if(mode_flag)
                return ROM[0x4000 * zerobank() + addr];
            
            return ROM[addr];
        }
        return 0xFF;       
    }

    void MBC1::writeByteToBank00(uint16_t addr, uint8_t what)
    {
        // Enable RAM
        if(addr >= 0x0000 && addr <= 0x1FFF)
            enable_ram = (what & 0b11111 == 0xA);
        // ROM bank
        else if(addr >= 0x2000 && addr <= 0x3FFF)
            if(what & 0b11111)
                rom_bank_number = what & bit_mask[rom_size];
            else
                rom_bank_number = 1;
    }

    uint8_t MBC1::readByteFromBankNN(uint16_t addr)
    {
        if(addr >= 0x4000 && addr <= 0x7FFF)
            return ROM[0x4000 * highbank() + (addr - 0x4000)];
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
        if(enable_ram && ram_size)
        {
            // If 32 kb and mode flag 1
            if(ram_size == 0x03 && mode_flag)
                return RAM[0x2000 * ram_bank_number + (addr - 0xA000)];
            // If 32 kb
            else if(ram_size == 0x03)
                return RAM[addr - 0xA000];
            else
                return RAM[(addr - 0xA000) % ((ram_size == 0x01) ? 2048 : 8192)];
        }
        return 0xFF;
    }

    void MBC1::writeByteToERAM(uint16_t addr, uint8_t what)
    {
        if(enable_ram && ram_size)
        {
            // If 32 kb and mode flag 1
            if(ram_size == 0x03 && mode_flag)
                RAM[0x2000 * ram_bank_number + (addr - 0xA000)] = what;
            // If 32 kb
            else if(ram_size == 0x03)
                RAM[addr - 0xA000] = what;
            else
                RAM[(addr - 0xA000) % ((ram_size == 0x01) ? 2048 : 8192)] = what;
        }
    }
}