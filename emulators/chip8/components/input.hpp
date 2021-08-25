#ifndef CHIP8_INPUT_H
#define CHIP8_INPUT_H

#include <iostream>
#include <SDL2/SDL.h>

namespace CHIP8
{
    class Input
    {
    private:
        // Keys' status
        SDL_Keycode keys[0x10] = {
            SDLK_x, SDLK_1, SDLK_2, SDLK_3,
            SDLK_q, SDLK_w, SDLK_e, SDLK_a,
            SDLK_s, SDLK_d, SDLK_z, SDLK_c,
            SDLK_4, SDLK_r, SDLK_f, SDLK_v
        };
        uint8_t key[0x10];
        uint8_t last_pressed;
        uint8_t key_count;

    public:
        // Resets the keys' status
        void resetKeys();

        // Gets mapped key
        SDL_Keycode getMappedKey(uint8_t index);

        // Gets a key's status
        uint8_t getKey(uint8_t who);

        // Gets the most recent keypress
        uint8_t getLastPressed();

        // Gets the number of currently pressed keys
        uint8_t getKeyCounter();

        // Sets a key's status
        void setKey(uint8_t who, uint8_t what);
    };
}

#endif