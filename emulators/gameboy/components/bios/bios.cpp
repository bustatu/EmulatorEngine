#include "bios.hpp"

namespace Gameboy
{
    BIOS::BIOS()
    {
        // Initialise 256 bytes of data
        data = new uint8_t[0x100];
    }

    uint8_t BIOS::readByte(uint16_t addr)
    {
        return data[addr];
    }

    bool BIOS::isLoaded()
    {
        return loaded;
    }

    void BIOS::load(std::string path)
    {
        // Open file for reading
        std::ifstream file(path.c_str(), std::ios::in | std::ios::binary | std::ios::ate);

        // Write info
        printf("{I}: Gameboy BIOS %s is being loaded...\n", path.c_str());

        // Get file size
        file.seekg(0, std::ios::end);
        int32_t fileSize = file.tellg();
        file.seekg(0, std::ios::beg);

        if(fileSize == -1)
            printf("\033[1;31m{E}: File at path %s does not exist!\n\033[0m", path.c_str());
        else if(fileSize != 0x100)
            printf("\033[1;31m{E}: File at path %s does not have the correct size! Excpected %d, got %d!\n\033[0m", path.c_str(), 0x100, fileSize);
        else
        {
            // Read the BIOS
            file.read(reinterpret_cast<char*>(data), fileSize);

            printf("{I}: BIOS with size %d loaded successfully!\n", fileSize);
            loaded = true;
        }
    }
}