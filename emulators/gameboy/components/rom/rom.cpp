#include "rom.hpp"

namespace Gameboy
{
    uint16_t ROM::readWord(uint16_t addr)
    {
        // Readings return 0xFFFF if nothing is loaded
        if(data == nullptr)
            return 0xFFFF;
        return (data[addr + 1] << 8) | data[addr];
    }

    uint8_t ROM::readByte(uint16_t addr)
    {
        // Readings return 0xFF if nothing is loaded
        if(data == nullptr)
            return 0xFF;
        return data[addr];
    }

    void ROM::load(std::string path)
    {
        // Open file for reading
        std::ifstream file(path.c_str(), std::ios::in | std::ios::binary | std::ios::ate);

        // Write info
        printf("{I}: Gameboy ROM %s is being loaded...\n", path.c_str());

        // Get file size
        file.seekg(0, std::ios::end);
        uint32_t fileSize = file.tellg();
        file.seekg(0, std::ios::beg);
        data = new uint8_t[fileSize];

        if(fileSize == -1)
        {
            printf("\033[1;31m{E}: File at path %s does not exist!\n\033[0m", path.c_str());
        }
        else
        {
            if(data[0x147] != 0x00)
            {
                printf("\033[1;31m{E}: Unsupported MBC! ROM might not work properly! MBC: %d\n\033[0m", data[0x147]);        
            }
            
            // Read the BIOS
            file.read(reinterpret_cast<char*>(data), fileSize);

            printf("{I}: ROM with size %d loaded successfully!\n", fileSize);
        }
    }
}