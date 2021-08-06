#ifndef BYTEPUSHER_VM_H
#define BYTEPUSHER_VM_H

#include "../../../window.h"
#include <SDL2/SDL.h>
#include <iostream>
#include <cstring>

class BYTEPUSHER_VM
{
private:
    // VM memory
    uint8_t *ram;
    uint32_t ram_size = 0xFFFFFF;

    // Color palette
    SDL_Color palette[256];

    // Keys' status
    SDL_Keycode keys[0x10] = {
        SDLK_x, SDLK_1, SDLK_2, SDLK_3,
        SDLK_q, SDLK_w, SDLK_e, SDLK_a,
        SDLK_s, SDLK_d, SDLK_z, SDLK_c,
        SDLK_4, SDLK_r, SDLK_f, SDLK_v
    };
    uint8_t* key;

    // Emulator state
    uint32_t state;

    // Frame counter
    double counter;

    // Frequency fixed at 60Hz
    double freq = 1.0 / 60;

    // Gets value at PC with length n
    uint32_t getVal(uint32_t pc, uint32_t n);

public:
    // Constructor
    BYTEPUSHER_VM();

    // Boot the VM
    void boot();

    // Update the VM
    void update(double dt, Window* win);

    // Draws to the output texture
    void draw(SDL_Texture* output, SDL_Renderer* tool);

    // Gets the VM state
    uint32_t getState();

    // Write buffer to RAM
    int8_t writeBuffer(uint16_t offset, uint8_t* buffer, uint32_t size);
};

#endif