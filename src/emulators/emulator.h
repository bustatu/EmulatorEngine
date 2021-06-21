#ifndef EMULATOR_H
#define EMULATOR_H

#include <iostream>
#include <SDL.h>
#include "../window/window.h"

// Template for the emulators that are run in the same thread as the app
class UnthreadedEmulator
{
public:
    virtual void input(SDL_Event event) = 0;
    virtual void load(std::string path = "") = 0;
    virtual void update(float deltaTime = 0) = 0;
    virtual void draw(Window* win) = 0;
    virtual void quit() = 0;
    virtual bool isRunning() = 0;
};

#endif