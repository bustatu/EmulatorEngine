#ifndef CHIP8_H
#define CHIP8_H

#include "../../state.h"
#include "../../gui/gui.h"
#include "components/vm.h"
#include <fstream>

class CHIP8_EMU : public State
{
private:
    // VM handling
    CHIP8_VM* vm = nullptr;

    // Texture which is going to be drawn to the screen
    SDL_Texture* output = nullptr;

    // Debug font and text
    Font debugFont;
    Text debugKeyText[0x10];
    int8_t debugKeyIndexes[0x10] = {
        0x1, 0x2, 0x3, 0xC,
        0x4, 0x5, 0x6, 0xD,
        0x7, 0x8, 0x9, 0xE,
        0xA, 0x0, 0xB, 0xF
    };

    // Debug output
    bool debug = false;

    // Draw VM keys in debug mode
    void drawVMKeys();

public:
    // Load the ROM from a path
    void load(std::string path);

    // Init the emulator
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