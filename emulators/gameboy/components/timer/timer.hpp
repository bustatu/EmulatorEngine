#ifndef GAMEBOY_TIMER_H
#define GAMEBOY_TIMER_H

#include "../bus/bus.hpp"

namespace Gameboy
{
    class Timer
    {
    private:
        // Bus handler
        Bus* bus = nullptr;

        // Timer handlings
        uint32_t freq, counter;

        // Auxiliary functions
        uint8_t get_bit(uint8_t who, uint8_t which);

    public:
        // Attach the bus to the timer
        void attachBus(Bus* newBus);

        // Update
        void update();
    };
}

#endif