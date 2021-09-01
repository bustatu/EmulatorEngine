#ifndef GAMEBOY_H
#define GAMEBOY_H

#include "../../state.hpp"
#include "components/components.hpp"
#include <SDL2/SDL.h>

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
        SDL_Keycode keys[8] = { SDLK_d, SDLK_a, SDLK_w, SDLK_s, SDLK_n, SDLK_m, SDLK_k, SDLK_l };

        // Auxiliary functions
        void updateJoypad();

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