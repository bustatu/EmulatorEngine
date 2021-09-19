#include "rom.hpp"

namespace Gameboy
{
    uint8_t ROM::readByte(uint16_t addr)
    {
        // Readings return 0xFF if nothing is loaded
        if(!loaded || data == nullptr)
            return 0xFF;

        // Do readings depending on the read loction
        if(addr >= 0x0000 && addr <= 0x3FFF)
            return data -> readByteFromBank00(addr);
        if(addr >= 0x4000 && addr <= 0x7FFF)
            return data -> readByteFromBankNN(addr);
        if(addr >= 0xA000 && addr <= 0xBFFF)
            return data -> readByteFromERAM(addr);
            
        // Unknown location
        return 0xFF;
    }

    void ROM::writeByte(uint16_t addr, uint8_t val)
    {
        // Don't do anything
        if(!loaded || data == nullptr)
            return;
        
        // Do readings depending on the read loction
        if(addr >= 0x0000 && addr <= 0x3FFF)
            data -> writeByteToBank00(addr, val);
        if(addr >= 0x4000 && addr <= 0x7FFF)
            data -> writeByteToBankNN(addr, val);
        if(addr >= 0xA000 && addr <= 0xBFFF)
            data -> writeByteToERAM(addr, val);
    }

    void ROM::load(std::string path)
    {
        // Open file for reading
        std::ifstream file(path.c_str(), std::ios::in | std::ios::binary | std::ios::ate);

        // Write info
        printf("{I}: Gameboy ROM %s is being loaded...\n", path.c_str());

        // Get file size
        file.seekg(0, std::ios::end);
        int32_t fileSize = file.tellg();
        file.seekg(0, std::ios::beg);
        uint8_t *dataFile = new uint8_t[fileSize];

        if(fileSize == -1)
            printf("\033[1;31m{E}: File at path %s does not exist!\n\033[0m", path.c_str());
        else
        {
            // Read the BIOS
            file.read(reinterpret_cast<char*>(dataFile), fileSize);

            // MBC check
            switch(dataFile[0x147])
            {
                case 0x00:
                    data = new MBC0();
                    data -> typeflag = 0x00;
                    printf("{I}: Supported MBC0 found!\n");
                    break;
                case 0x01:
                    data = new MBC1();
                    data -> typeflag = 0x00;
                    printf("{I}: Supported MBC1 found!\n");
                    break;
                case 0x02:
                    data = new MBC1();
                    data -> typeflag = 0x01;
                    printf("{I}: Supported MBC1 + RAM found!\n");               
                    break;
                case 0x03:
                    data = new MBC1();
                    data -> typeflag = 0x03;
                    printf("{I}: Supported MBC1 + RAM + BATTERY found!\n");  
                    break;
                case 0x13:
                    data = new MBC3();
                    data -> typeflag = 0x03;
                    printf("\033[1;31m{I}: MBC3 + RAM + BATTERY is incomplete!\n\033[0m");
                    break;
                case 0x19:
                    data = new MBC5();
                    printf("{I}: Supported MBC5 found!\n");
                    break;
                default:
                    printf("\033[1;31m{E}: Unsupported MBC! ROM might not work properly! MBC: %02X\n\033[0m", dataFile[0x147]);
                    printf("{I}: Defaulting to MBC 0...\n");
                    data = new MBC0();
                    break;
            }

            // Get file name from the path (if required)
            uint32_t pos = path.find_last_of('/');
            if(pos != std::string::npos)
                path = path.substr(pos + 1, path.size() - pos);

            // Remove extension (if any)
            pos = path.find_last_of('.');
            if(pos != std::string::npos)
                path = path.substr(0, pos);

            // Init the ROM
            data -> init(dataFile, fileSize, path);

            // Mark as loaded
            loaded = true;
            printf("{I}: ROM with size %d loaded successfully!\n", fileSize);

            // Delete the loaded memory
            delete[] dataFile;
        }
    }

    void ROM::destroy()
    {
        loaded = false;
        delete data;
    }
}