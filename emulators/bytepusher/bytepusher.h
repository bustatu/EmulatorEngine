#ifndef BYTEPUSHER_H
#define BYTEPUSHER_H

#include "../../state.h"
#include "components/vm.h"
#include <fstream>

class BYTEPUSHER_EMU : public State
{
private:
    // VM handling
    BYTEPUSHER_VM* vm = nullptr;

    // Texture which is going to be drawn to the screen
    SDL_Texture* output = nullptr;

public:
    // Load the ROM from a path
    void load(std::string path);

    // Init the emulator on new state
    virtual void init();

    // Resume the emulator
    virtual void resume();

    // Stop the emulator
    virtual void pause();

    // Main update function
    virtual void update(double dt);

    // Main draw function
    virtual void draw();
};

#endif