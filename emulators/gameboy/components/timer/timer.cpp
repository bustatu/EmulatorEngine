#include "timer.hpp"

#define get_bit(who, which) (((who) >> (which)) & 1)

namespace Gameboy
{
    void Timer::writeByte(uint16_t addr, uint8_t val)
    {
        // DIV is always reset to 0 when written
        if(addr - 0xFF04 == 0)
            DIV = 0, divcounter = timacounter = 0;
        else
            timers[addr - 0xFF04] = val;
    }

    uint8_t Timer::readByte(uint16_t addr)
    {
        // Return timer register
        if(addr >= 0xFF04 && addr <= 0xFF07)
            return timers[addr - 0xFF04];
        return 0xFF;
    }

    bool Timer::needsInterrupt()
    {
        bool before = interruptFlag;
        interruptFlag = false;
        return before;
    }

    void Timer::update()
    {
        // Update counter
        divcounter++;

        // Increment DIV
        while(divcounter >= 256)
            DIV++, divcounter -= 256;
        
        // If timer is enabled
        if(get_bit(TAC, 2))
        {
            // Get clock frequency
            switch(TAC & 0x3)
            {
                case 0x00: freq = 1024; break;
                case 0x01: freq =  16 ; break;
                case 0x02: freq =  64 ; break;
                case 0x03: freq =  256; break;
            }

            // Update timer
            timacounter++;
            while(timacounter >= freq)
            {
                // If TIMA can be incremented
                if(TIMA < 0xFF)
                    TIMA++;
                else
                {
                    // Reset value and request interrupt
                    TIMA = TMA;

                    // Mark as in need to request interrupt
                    interruptFlag = true;
                }

                timacounter -= freq;
            }
        }
    }
}