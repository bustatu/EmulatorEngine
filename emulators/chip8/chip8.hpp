#ifndef CHIP8_H
#define CHIP8_H

#include <fstream>
#include <cmath>
#include "../../state.hpp"
#include "../../gui/gui.hpp"
#include "components/input.hpp"
#include "components/dynarec/vm.hpp"
#include "components/interpreter/vm.hpp"
#include "../../external/json/single_include/nlohmann/json.hpp"

namespace CHIP8
{
    class Emu : public State
    {
        private:
            // VM handling
            VM* vm = nullptr;
            VM_DYNAREC* vm_dynarec = nullptr;

            // Input handler
            Input input;

            // Graphics handler
            Graphics graphics;

            // Texture which is going to be drawn to the screen
            SDL_Texture* output = nullptr;

            // Create default json file config
            static void createDefaultConfig();

            // Apply default config
            void applyDefaultConfig();

            // Use dynarec for now
            bool use_dynarec = false;

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

            // Destroy the emulator after execution ended
            void destroy();
    };
}

#endif