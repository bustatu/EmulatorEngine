#ifndef GAMEBOY_H
#define GAMEBOY_H

#include "../../state.hpp"
#include "components/components.hpp"

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
        Timer* timer = nullptr;
        Joypad* joypad = nullptr;

        // Set clock frequency (4194304 Hz)
        uint32_t freq = 4194304;

        // Execution timing
        double executionTimer;

        // Output texture
        SDL_Texture* output;

        // Keys' mapping
        SDL_Keycode keys[6] = { SDLK_w, SDLK_s, SDLK_a, SDLK_d, SDLK_m, SDLK_k };

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

        // Skips BIOS
        void skip_bios();

        // Update function
        void update(double dt);

        // Main draw function
        void draw();
    };
}


#endif