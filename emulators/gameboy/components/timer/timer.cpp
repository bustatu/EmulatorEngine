#include "timer.hpp"

namespace Gameboy
{
    void Timer::attachBus(Bus* newBus)
    {
        // Attach bus to the timer
        bus = newBus;

        // Write default values to registers
        // DIV
        bus -> writeByte(0xFF04, 0x00);
        // TIMA
        bus -> writeByte(0xFF05, 0x00);
        // TMA
        bus -> writeByte(0xFF06, 0x00);
        // TAC
        bus -> writeByte(0xFF07, 0xF8);

        // Clock stuff
        counter = 0;
        freq = 1024;
    }

    uint8_t Timer::get_bit(uint8_t who, uint8_t which)
    {
        return ((who >> which) & 1);
    }

    void Timer::update()
    {
        // Increment DIV
        bus -> writeByte(0xFF04, bus -> readByte(0xFF04) + 1);

        // If timer is enabled
        if(get_bit(bus -> readByte(0xFF07), 2))
        {
            // Update the frequency if it is changed
            switch(bus -> readByte(0xFF07) & 0x3)
            {
                case 0x00: freq = 1024; break;
                case 0x01: freq =  16 ; break;
                case 0x02: freq =  64 ; break;
                case 0x03: freq =  256; break;
            }

            // Advance the timers
            counter++;
            while(counter >= freq)
            {
                // If TIMA can be incremented
                if(bus -> readByte(0xFF05) < 0xFF)
                    bus -> writeByte(0xFF05, bus -> readByte(0xFF05) + 1);
                else
                {
                    // Set TIMA to TMA
                    bus -> writeByte(0xFF05, bus -> readByte(0xFF06));
                    
                    // Request an interrupt
                    uint8_t IF = bus -> readByte(0xFF0F);
                    IF |= (1 << 2);
                    bus -> writeByte(0xFF0F, IF);
                }

                counter -= freq;
            }
        }
    }
}