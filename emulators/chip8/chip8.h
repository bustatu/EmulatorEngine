#ifndef CHIP8_H
#define CHIP8_H

#include "../../state.h"
#include "../../gui/gui.h"
#include "components/vm.h"
#include <fstream>

namespace CHIP8
{
    class Emu : public State
    {
        private:
            // VM handling
            CHIP8::VM* vm = nullptr;

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