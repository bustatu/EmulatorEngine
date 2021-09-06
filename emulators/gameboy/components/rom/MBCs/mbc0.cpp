#include "mbc0.hpp"

namespace Gameboy
{
    void MBC0::init(uint8_t* fileContents, uint32_t fileSize, std::string fileName)
    {
        // Copy all the contents in the ROM if the fileSize is small enough
        if(fileSize <= 0x8000)
            memcpy(ROM, fileContents, fileSize);
        else
        {
            // Copy only the correct ammount of bytes
            printf("{W}: Expected max 32678 bytes of ROM, loaded only the first 32678 bytes. Got %d bytes.\n", fileSize);
            memcpy(ROM, fileContents, 0x8000);
        }
    }

    uint8_t MBC0::readByteFromBank00(uint16_t addr)
    {
        if(addr >= 0x0000 && addr <= 0x3FFF)
            return ROM[addr];
        return 0xFF;       
    }

    void MBC0::writeByteToBank00(uint16_t addr, uint8_t what)
    {
        printf("{W}: Writing to the memory banks is not supported in MBC0! Addr: %04X, value: %04X\n", addr, what);
    }

    uint8_t MBC0::readByteFromBankNN(uint16_t addr)
    {
        if(addr >= 0x4000 && addr <= 0x7FFF)
            return ROM[addr];
        return 0xFF; 
    }

    void MBC0::writeByteToBankNN(uint16_t addr, uint8_t what)
    {
        printf("{W}: Writing to the memory banks is not supported in MBC0! Addr: %04X, value: %04X\n", addr, what);
    }

    uint8_t MBC0::readByteFromERAM(uint16_t addr)
    {
        addr -= 0xA000;
        if(addr >= 0x0000 && addr <= 0x1FFF)
            return RAM[addr];
        return 0xFF;
    }

    void MBC0::writeByteToERAM(uint16_t addr, uint8_t what)
    {
        addr -= 0xA000;
        if(addr >= 0x0000 && addr <= 0x1FFF)
            RAM[addr] = what;
    }

    MBC0::~MBC0()
    {
        
    }
}