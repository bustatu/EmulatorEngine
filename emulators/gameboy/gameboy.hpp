#ifndef GAMEBOY_H
#define GAMEBOY_H

#include <SDL2/SDL.h>
#include "../../state.hpp"
#include "components/components.hpp"
#include "../../external/json/single_include/nlohmann/json.hpp"

namespace Gameboy
{
    class Emu : public State
    {
    private:
        // Emulator components
        Bus bus;
        CPU cpu;
        BIOS bios;
        RAM ram;
        GPU gpu;
        ROM rom;
        Timer timer;
        Joypad joypad;

        // Set clock frequency (4194304 Hz)
        uint32_t freq = 4194304;

        // Execution timing
        double executionTimer;

        // Output texture
        SDL_Texture* output;

        // Keys' mapping
        SDL_Keycode keys[8] = { SDLK_d, SDLK_a, SDLK_w, SDLK_s, SDLK_n, SDLK_m, SDLK_k, SDLK_l };

        // Create default json file config
        static void createDefaultConfig();

        // Apply the default config to the curent state
        void applyDefaultConfig();

    public:

        // Load the ROM
        void loadROM(std::string path);

        // Init the emulator on new state
        void init();

        // Resume the emulator
        void resume();

        // Stop the emulator
        void pause();

        // Update function
        void update(double dt);

        // Main draw function
        void draw();
    };
}


#endif