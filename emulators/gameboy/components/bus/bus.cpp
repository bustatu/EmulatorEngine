#include "bus.hpp"

namespace Gameboy
{
    uint8_t Bus::readByte(uint16_t addr)
    {
        // BIOS and ROM area
        if(addr >= 0x0000 && addr <= 0x00FF)
        {
            // Cartridge control
            if(readByte(0xFF50) == 0x00)
                return bios -> readByte(addr);
            else
                return rom -> readByte(addr);
        }
        // ROM only area
        else if(addr >= 0x0100 && addr <= 0x7FFF)
            return rom -> readByte(addr); 
        // VRAM
        else if(addr >= 0x8000 && addr <= 0x9FFF)
            return ram -> readByte(addr);
        // External RAM
        else if(addr >= 0xA000 && addr <= 0xBFFF)
            return rom -> readByte(addr);
        // WRAM
        else if(addr >= 0xC000 && addr <= 0xCFFF)
            return ram -> readByte(addr);
        // WRAM 2
        else if(addr >= 0xD000 && addr <= 0xDFFF)
            return ram -> readByte(addr);
        // Mirror of of WRAM and part of WRAM 2
        else if(addr >= 0xE000 && addr <= 0xFDFF)
            return ram -> readByte(addr - 0x1000);
        // OAM
        else if(addr >= 0xFE00 && addr <= 0xFE9F)
            return ram -> readByte(addr - 0x1000);
        // Unusable
        else if(addr >= 0xFEA0 && addr <= 0xFEFF)
            return 0x00;
        // Timer registers
        else if(addr >= 0xFF04 && addr <= 0xFF07)
            return timer -> readByte(addr);
        // Others
        else if(addr <= 0xFFFF)
        {
            // Joypad output
            if(addr == 0xFF00)
                return joypad -> getState();
            return ram -> readByte(addr);
        }

        // Invalid
        return 0xFF;
    }

    uint16_t Bus::readWord(uint16_t addr)
    {
        // 2 byte reads
        return (readByte(addr + 1) << 8) | readByte(addr);
    }

    void Bus::writeByte(uint16_t addr, uint8_t val)
    {
        // ROM banks
        if(addr >= 0x0000 && addr <= 0x7FFF)
            rom -> writeByte(addr, val);
        // VRAM
        else if(addr >= 0x8000 && addr <= 0x9FFF)
            ram -> writeByte(addr, val);
        // External RAM
        else if(addr >= 0xA000 && addr <= 0xBFFF)
            rom -> writeByte(addr, val);
        // WRAM
        else if(addr >= 0xC000 && addr <= 0xCFFF)
            ram -> writeByte(addr, val);
        // WRAM 2
        else if(addr >= 0xD000 && addr <= 0xDFFF)
            ram -> writeByte(addr, val);
        // Mirror of of WRAM and part of WRAM 2
        else if(addr >= 0xE000 && addr <= 0xFDFF)
            ram -> writeByte(addr - 0x1000, val);
        // OAM
        else if(addr >= 0xFE00 && addr <= 0xFE9F)
            ram -> writeByte(addr - 0x1000, val);       
        // Unusable
        else if(addr >= 0xFEA0 && addr <= 0xFEFF)
            printf("{W}: Write to unusable area at address %04X, value %02X.\n", addr, val);
        // Timer registers
        else if(addr >= 0xFF04 && addr <= 0xFF07)
            return timer -> writeByte(addr, val);
        // Write to the other spaces in RAM
        else if(addr <= 0xFFFF)
        {
            // Joypad output
            if(addr == 0xFF00)
                joypad -> handleWrite(val);
            ram -> writeByte(addr, val);
        }
    }

    void Bus::writeWord(uint16_t addr, uint16_t val)
    {
        // 2 byte writes
        writeByte(addr, val & 0xFF);
        writeByte(addr + 1, val >> 8);
    }
}