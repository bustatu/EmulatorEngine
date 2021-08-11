#include "bios.h"

Gameboy_BIOS::Gameboy_BIOS()
{
    // Initialise 256 bytes of data
    data = new uint8_t[0x100];
}

uint16_t Gameboy_BIOS::readWord(uint16_t addr)
{
    return (data[addr + 1] << 8) | data[addr];
}

uint8_t Gameboy_BIOS::readByte(uint16_t addr)
{
    return data[addr];
}

void Gameboy_BIOS::load(std::string path)
{
    // Open file for reading
    std::ifstream file(path.c_str(), std::ios::in | std::ios::binary | std::ios::ate);

    // Write info
    printf("{I}: Gameboy BIOS %s is being loaded...\n", path.c_str());

    // Get file size
    file.seekg(0, std::ios::end);
    uint32_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    if(fileSize == -1)
    {
        printf("\033[1;31m{E}: File at path %s does not exist!\n\033[0m", path.c_str());
        loaded = false;
    }
    else if(fileSize != 0x100)
    {
        printf("\033[1;31m{E}: File at path %s does not have the correct size! Excpected %d, got %d!\n\033[0m", path.c_str(), 0x100, fileSize);
        loaded = false;
    }
    else
    {
        // Read the BIOS
        file.read(reinterpret_cast<char*>(data), fileSize);

        printf("{I}: BIOS with size %d loaded successfully!\n", fileSize);
        loaded = true;
    }
}