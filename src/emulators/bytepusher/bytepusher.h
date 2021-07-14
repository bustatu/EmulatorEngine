#ifndef EMU_BYTEPUSHER_H
#define EMU_BYTEPUSHER_H

#include "../emulator.h"
#include "../../engine/audio.h"
#include "../../engine/window.h"
#include <SDL2/SDL.h>
#include <iostream>
#include <fstream>

class BytePusherEmu : public Emulator
{
private:
    // VM memory
    uint8_t *memory;
    uint32_t getVal(uint32_t pc,uint32_t length);

    // VM management
    float counter = 0;
    bool isQuit = false;
    void innerLoop();

    // Output
    SDL_Surface* result = SDL_CreateRGBSurface(0, 256, 256, 32, 0, 0, 0, 0);
    uint32_t pallete[256];
    Audio* sound;

    // Input
    SDL_Keycode keys[0x10] = {
        SDLK_x,
        SDLK_1, SDLK_2, SDLK_3,
        SDLK_q, SDLK_w, SDLK_e,
        SDLK_a, SDLK_s, SDLK_d,
        SDLK_z,         SDLK_c,
        SDLK_4,
        SDLK_r,
        SDLK_f,
        SDLK_v     
    };
    void updateKey(SDL_Keycode key, int value);

public:
    // Emu management
    BytePusherEmu();
    bool isRunning() override;
    void quit() override;
    void load(std::string path) override;
    void update(float deltaTime) override;
    void draw(Window* win) override;
};

#endif