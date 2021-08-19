#include "bus.h"

namespace Gameboy
{
    void Bus::attachBIOS(BIOS* newBIOS)
    {
        bios = newBIOS;
    }

    void Bus::attachRAM(RAM* newRAM)
    {
        ram = newRAM;
    }

    void Bus::attachROM(ROM* newROM)
    {
        rom = newROM;
    }

    uint8_t Bus::readByte(uint16_t addr)
    {
        /*// BIOS area
        if(addr >= 0x0000 && addr <= 0x3FFF)
        {
            // TODO: ROMs load from this area too.
            return bios -> readByte(addr);
        }
        // ROM area
        else*/
        
        if(addr <= 0x7FFF)
        {
            return rom -> readByte(addr);       
        }
        else
        {      
            return ram -> readByte(addr);
        }

        // Not implemented
        printf("\033[1;31m{E}: Unimplemented readByte at address %04X\033[0m\n", addr);
        exit(0);
    }

    uint16_t Bus::readWord(uint16_t addr)
    {
        /*// BIOS area
        if(addr >= 0x0000 && addr <= 0x3FFE)
            return bios -> readWord(addr);
        // ROM area
        else*/
        if(addr <= 0x7FFF)
            return rom -> readWord(addr);       
        // RAM area
        else 
            return ram -> readWord(addr);

        // Not implemented
        printf("\033[1;31m{E}: Unimplemented readWord at address %04X\033[0m\n", addr);
        exit(0);
    }

    void Bus::writeByte(uint16_t addr, uint8_t val)
    {
        if(addr > 0x7FFF)
            ram -> writeByte(addr, val);
        else
            printf("\033[1;31m{E}: Unimplemented writeByte at address %04X, value %04X \033[0m\n", addr, val);
    }

    void Bus::writeWord(uint16_t addr, uint16_t val)
    {
        if(addr > 0x7FFF)
            ram -> writeWord(addr, val);
        else
            printf("\033[1;31m{E}: Unimplemented writeWord at address %04X, value %04X \033[0m\n", addr, val);
    }
}