#include "mbc3.hpp"

#define get_bit(who, which) (((who) >> (which)) & 1)
#define set_bit(who, which, what) who = (what) ? ((who) | (1 << (which))) : ((who) & ~(1 << (which)))

namespace Gameboy
{
    void MBC3::init(uint8_t* fileContents, uint32_t fileSize)
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
        // Enable RAM
        if(addr >= 0x0000 && addr <= 0x1FFF)
            enable_ram = ((what & 0b1111) == 0xA);
        // ROM bank
        else if(addr >= 0x2000 && addr <= 0x3FFF)
        {
            if(what & 0b1111111)
                rom_bank_number = what & 0b1111111;
            else
                rom_bank_number = 1;
        }
    }

    uint8_t MBC3::readByteFromBankNN(uint16_t addr)
    {
        if(addr >= 0x4000 && addr <= 0x7FFF)
            return ROM[0x4000 * rom_bank_number + (addr - 0x4000)];
        return 0xFF; 
    }

    void MBC3::writeByteToBankNN(uint16_t addr, uint8_t what)
    {
        // RAM Bank
        if(addr >= 0x4000 && addr <= 0x5FFF)
        {
            if(what >= 0x00 && what <= 0x03)
                rtc_flag = false, ram_bank_number = what;
            else if(what >= 0x08 && what <= 0x0C)
                rtc_flag = true;
        }
        // RTC data
        else if(addr >= 0x6000 && addr <= 0x7FFF)
        {
            // Stop register updates
            if(what == 0x00)
            {

            }
            // Update the registers
            else if(what == 0x01)
            {

            }
        }
    }

    uint8_t MBC3::readByteFromERAM(uint16_t addr)
    {
        if(enable_ram)
        {
            if(rtc_flag)
                return 0xFF;
            else
            {
                if(ram_size)
                    return RAM[0x2000 * ram_bank_number + (addr - 0xA000)];
                else
                    return 0xFF;
            }
        }
        else if(rtc_flag) {
            return 0xFF;
        }
        return 0xFF;
    }

    void MBC3::writeByteToERAM(uint16_t addr, uint8_t what)
    {
        if(enable_ram && ram_size)
        {
            if(ram_size)
                RAM[0x2000 * ram_bank_number + (addr - 0xA000)] = what;
        }
    }
}