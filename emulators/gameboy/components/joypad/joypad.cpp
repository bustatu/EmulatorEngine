#include "joypad.hpp"

namespace Gameboy
{
    void Joypad::attachBus(Bus* newBus)
    {
        // Attach bus to the timer
        bus = newBus;

        // Mark all buttons as released
        bus -> writeByte(0xFF00, 0xFF);
    }

    void Joypad::press(int32_t button)
    {
        //printf("{I}: Key %d has been pressed!\n", button);
    }

    void Joypad::release(int32_t button)
    {
        //printf("{I}: Key %d has been released!\n", button);
    }

    void Joypad::set_bit(uint8_t &who, uint8_t which, uint8_t what)
    {
        if(what)
            who |= (1 << which);
        else
            who &= ~(1 << which);
    }

    uint8_t Joypad::get_bit(uint8_t who, uint8_t which)
    {
        return ((who >> which) & 1);
    }
}