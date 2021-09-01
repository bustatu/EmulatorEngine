#ifndef GAMEBOY_JOYPAD_H
#define GAMEBOY_JOYPAD_H

#include <iostream>

namespace Gameboy
{
    class Joypad
    {
    private:
        // Auxiliary functions
        uint8_t get_bit(uint8_t who, uint8_t which);
        void set_bit(uint8_t &who, uint8_t which, uint8_t what);

        // Joypad handling
        uint8_t state = 0xFF;
        uint8_t P1 = 0xC0;

    public:
        // Get state as byte (used in GB bus)
        uint8_t getState();

        // If we need to make a joypad interrupt
        bool doInter = false;

        // Handles a write to the joypad register (used in GB bus)
        void handleWrite(uint8_t what);

        // Press a button
        void press(int32_t button);
        
        // Release a button
        void release(int32_t button);
    };
}

#endif