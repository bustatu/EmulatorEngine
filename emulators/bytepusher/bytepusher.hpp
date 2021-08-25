#ifndef BYTEPUSHER_H
#define BYTEPUSHER_H

#include "../../state.hpp"
#include "components/vm.hpp"
#include <fstream>

namespace Bytepusher
{
    class Emu : public State
    {
    private:
        // VM handling
        VM* vm = nullptr;

        // Texture which is going to be drawn to the screen
        SDL_Texture* output = nullptr;

    public:
        // Load the ROM from a path
        void load(std::string path);

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