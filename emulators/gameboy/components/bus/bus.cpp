#include "bus.hpp"

namespace Gameboy
{
    uint8_t Bus::readByte(uint16_t addr)
    {
        switch(addr)
        {
            // BIOS and ROM area
            case 0x0000 ... 0x00FF:
                // Cartridge control
                if(readByte(0xFF50) == 0x00)
                    return bios -> readByte(addr);
                else
                    return rom -> readByte(addr);
                break;
            
            // ROM only area
            case 0x1000 ... 0x7FFF:
                return rom -> readByte(addr); 
                break;
            
            // VRAM
            case 0x8000 ... 0x9FFF:
                return ram -> readByte(addr);
                break;

            // External RAM
            case 0xA000 ... 0xBFFF:
                return rom -> readByte(addr);
                break;

            // WRAM 1
            case 0xC000 ... 0xCFFF:
                return ram -> readByte(addr);
                break;

            // WRAM 2
            case 0xD000 ... 0xDFFF:
                return ram -> readByte(addr);
                break;

            // Mirror of WRAM 1 and WRAM 2
            case 0xE000 ... 0xFDFF:
                return ram -> readByte(addr - 0x1000);
                break;

            // OAM
            case 0xFE00 ... 0xFE9F:
                return ram -> readByte(addr - 0x1000);
                break;

            // Unusable
            case 0xFEA0 ... 0xFEFF:
                return 0x00;
                break;

            // Joypad registers
            case 0xFF00:
                return joypad -> readByte(addr);
                break;

            // Timer registers
            case 0xFF04 ... 0xFF07:
                return timer -> readByte(addr);
                break;

            // GPU registers
            case 0xFF40 ... 0xFF4A:
                return gpu -> readByte(addr);
                break;

            // Other RAM locations
            default:
                return ram -> readByte(addr);
                break;
        }
    }

    void Bus::writeByte(uint16_t addr, uint8_t val)
    {
        switch(addr)
        {
            // ROM banks
            case 0x0000 ... 0x7FFF:
                rom -> writeByte(addr, val);
                break;

            // VRAM
            case 0x8000 ... 0x9FFF:
                ram -> writeByte(addr, val);
                break;

            // External RAM
            case 0xA000 ... 0xBFFF:
                rom -> writeByte(addr, val);
                break;

            // WRAM 1
            case 0xC000 ... 0xCFFF:
                ram -> writeByte(addr, val);
                break;

            // WRAM 2
            case 0xD000 ... 0xDFFF:
                ram -> writeByte(addr, val);
                break;

            // Mirror of WRAM 1 and WRAM 2
            case 0xE000 ... 0xFDFF:
                ram -> writeByte(addr - 0x1000, val);
                break;

            // OAM
            case 0xFE00 ... 0xFE9F:
                ram -> writeByte(addr - 0x1000, val);
                break;

            // Unusable area
            case 0xFEA0 ... 0xFEFF:
                printf("{W}: Write to unusable area at address %04X, value %02X.\n", addr, val);
                break;

            // Joypad register
            case 0xFF00:
                joypad -> writeByte(addr, val);
                break;

            // Timer registers
            case 0xFF04 ... 0xFF07:
                timer -> writeByte(addr, val);
                break;

            // GPU registers
            case 0xFF40 ... 0xFF4B:
                gpu -> writeByte(addr, val);
                break;

            // Upper RAM
            default:
                ram -> writeByte(addr, val);
                break;
        }
    }
}