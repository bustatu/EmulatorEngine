#ifndef GAMEBOY_JOYPAD_H
#define GAMEBOY_JOYPAD_H

#include <iostream>

namespace Gameboy
{
    class Joypad
    {
    private:
        // Joypad handling
        uint8_t state = 0xFF;
        uint8_t P1 = 0xC0;
    
        // Interrupt flag
        bool interrputFlag = false;

    public:
        // Get state as byte (used in GB bus)
        uint8_t readByte(uint16_t addr);

        // Handles a write to the joypad register (used in GB bus)
        void writeByte(uint16_t addr, uint8_t what);

        // Check if needs interrupt (RESETS THE FLAG)
        bool needsInterrupt();

        // Updates a key's value
        void updateButton(uint8_t button, uint8_t value);
    };
}

#endif