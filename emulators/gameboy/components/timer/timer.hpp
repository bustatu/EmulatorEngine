#ifndef GAMEBOY_TIMER_H
#define GAMEBOY_TIMER_H

#include <iostream>

namespace Gameboy
{
    class Timer
    {
    private:
        // Timer registers
        uint8_t timers[4] = {0x00, 0x00, 0x00, 0xF8};
        uint8_t &DIV = timers[0], &TIMA = timers[1], &TMA = timers[2], &TAC = timers[3];

        // TIMA and DIV counters
        uint32_t timacounter = 0, divcounter = 0;

        // TIMA frequency
        uint32_t freq = 1024;

        // If an interrupt needs to be requested
        bool interruptFlag = false;

    public:
        // Write byte to timer
        void writeByte(uint16_t addr, uint8_t val);

        // Read byte from timer
        uint8_t readByte(uint16_t addr);

        // Check if needs interrupt (RESETS THE FLAG)
        bool needsInterrupt();

        // Forcefully resets DIV
        void resetDIV();

        // Update executed each CPU cycle
        void update();
    };
};

#endif