#ifndef GAMEBOY_H
#define GAMEBOY_H

#include "../../state.h"
#include "components/components.h"

class GAMEBOY_EMU : public State
{
private:
    // Emulator components
    Gameboy_Bus* bus = nullptr;
    Gameboy_CPU* cpu = nullptr;
    Gameboy_BIOS* bios = nullptr;

public:
    // Load the BIOS
    void loadBIOS(std::string path);

    // Init the emulator on new state
    void init();

    // Resume the emulator
    void resume();

    // Stop the emulator
    void pause();

    // Main update function
    void update(double dt);

    // Main draw function
    void draw();
};

#endif