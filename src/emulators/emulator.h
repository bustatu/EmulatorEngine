#ifndef EMULATOR_H
#define EMULATOR_H

#include <iostream>
#include <SDL.h>
#include "../engine/window.h"

// Template for any emulator to be able to interact with the UI
class Emulator
{
public:
    virtual void load(std::string path = "") = 0;
    virtual void update(float deltaTime = 0) = 0;
    virtual void draw(Window* win) = 0;
    virtual void quit() = 0;
    virtual bool isRunning() = 0;
};

#endif