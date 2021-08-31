#ifndef GAMEBOY_JOYPAD_H
#define GAMEBOY_JOYPAD_H

#include "../bus/bus.hpp"

namespace Gameboy
{
    class Joypad
    {
    private:
        // Bus handler
        Bus* bus = nullptr;

        // Auxiliary functions
        uint8_t get_bit(uint8_t who, uint8_t which);
        void set_bit(uint8_t &who, uint8_t which, uint8_t what);

    public:
        // Attach the bus to the timer
        void attachBus(Bus* newBus);

        // Update the joypad to the bus
        void update();

        // Press a button
        void press(int32_t button);
        
        // Release a button
        void release(int32_t button);
    };
}

#endif