#ifndef NES_H
#define NES_H

#include "../../state.hpp"
#include "../../gui/gui.hpp"
#include <fstream>

namespace NES
{
    class Emu : public State
    {
        private:
            // Texture which is going to be drawn to the screen
            SDL_Texture* output = nullptr;

        public:
            // Load the ROM from a path
            void load(std::string path);

            // Init the emulator
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