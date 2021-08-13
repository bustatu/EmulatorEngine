#ifndef GAMEBOY_H
#define GAMEBOY_H

#include "../../state.h"
#include "components/components.h"

namespace Gameboy
{
    class Emu : public State
    {
    private:
        // Emulator components
        Bus* bus = nullptr;
        CPU* cpu = nullptr;
        BIOS* bios = nullptr;
        RAM* ram = nullptr;
        GPU* gpu = nullptr;
        ROM* rom = nullptr;

        // Output texture
        SDL_Texture* output;

    public:
        // Load the BIOS
        void loadBIOS(std::string path);

        // Load the ROM
        void loadROM(std::string path);

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
}


#endif