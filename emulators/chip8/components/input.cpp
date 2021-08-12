#include "input.h"

namespace CHIP8
{
    void Input::resetKeys()
    {
        // Set the entire array to 0
        memset(key, 0, 0x10);

        // No keys have been pressed so far
        last_pressed = 0;
        key_count = 0;
    }

    uint8_t Input::getKey(uint8_t who)
    {
        // Return the value
        return key[who];
    }

    void Input::setKey(uint8_t who, uint8_t what)
    {
        // Set the key and update the keycount
        if(key[who] != what)
            key_count += what - key[who];
        key[who] = what;
        last_pressed = (what ? who : last_pressed);
    }

    SDL_Keycode Input::getMappedKey(uint8_t index)
    {
        // Returns key mapped at position index
        return keys[index];
    }

    uint8_t Input::getLastPressed()
    {
        // Return the last key pressed
        return last_pressed;
    }

    uint8_t Input::getKeyCounter()
    {
        // Return the keycount
        return key_count;
    }
}