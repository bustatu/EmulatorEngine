#include "joypad.hpp"

namespace Gameboy
{
    void Joypad::press(int32_t button)
    {
        bool pressed = !get_bit(state, button);

        set_bit(state, button, 0);

        doInter = ((button > 3 && !get_bit(P1, 5)) || (button <= 3 && !get_bit(P1, 4))) && !pressed;
    }

    void Joypad::release(int32_t button)
    {
        set_bit(state, button, 1);
    }

    uint8_t Joypad::getState()
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

    void Joypad::handleWrite(uint8_t what)
    {
        P1 = what & 0x30;
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