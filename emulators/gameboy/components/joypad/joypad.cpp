#include "joypad.hpp"

#define get_bit(who, which) (((who) >> (which)) & 1)
#define set_bit(who, which, what) who = (what) ? ((who) | (1 << (which))) : ((who) & ~(1 << (which)))

namespace Gameboy
{
    uint8_t Joypad::readByte(uint16_t addr)
    {
        if(addr == 0xFF00)
        {
            uint8_t jp_reg = P1;
            jp_reg |= 0xCF;

            // Action buttons
            if(!get_bit(jp_reg, 5))
                jp_reg &= 0xF0 | (state >> 4);

            // Directional buttons
            if(!get_bit(jp_reg, 4))
                jp_reg &= 0xF0 | (state & 0x0F);

            return jp_reg;
        }
        return 0xFF;
    }

    bool Joypad::isButtonPressed()
    {
        return !(state == 0xFF);
    }

    void Joypad::updateButton(uint8_t button, uint8_t value)
    {
        // If key has been pressed, decide if that requires an interrupt (has been pressed for the first time)
        if(value == 1)
            interrputFlag = ((button > 3 && !get_bit(P1, 5)) || (button <= 3 && !get_bit(P1, 4))) && !get_bit(state, button);

        set_bit(state, button, value);
    }

    bool Joypad::needsInterrupt()
    {
        bool before = interrputFlag;
        interrputFlag = false;
        return before;
    }

    void Joypad::writeByte(uint16_t addr, uint8_t what)
    {
        if(addr == 0xFF00)
            P1 = what & 0x30;
    }
}