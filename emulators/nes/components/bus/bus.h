#ifndef NES_BUS_H
#define NES_BUS_H

#include <iostream>
#include <array>

namespace NES
{
    class Bus
    {
    public:
        Bus();
        ~Bus();

        // Fake RAM for testing now
        std::array<uint8_t, 64 * 1024> ram;

        // Write byte
        void write(uint16_t addr, uint8_t data);

        // Read byte
        uint8_t read(uint16_t addr, bool flagReadOnly = false);
    };
}

#endif