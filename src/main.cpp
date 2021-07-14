#include <iostream>
#include "engine/window.h"
#include "emulators/bytepusher/bytepusher.h"

int main(int argc, char* argv[])
{
    // Get ROM name
    std::string rom_name;
    std::cout << "{S}: Please write the ROM name.\n{S}: Roms can be found in the roms/bytepusher/ folder.\n{Q}: ROM: ";
    std::cin >> rom_name;
    if(rom_name == "") rom_name = "nyan.bp";

    // Create emu
    Emulator* emu = new BytePusherEmu();

    // Load the ROM
    emu -> load("roms/bytepusher/" + rom_name);

    // If emu did not fail
    if(emu -> isRunning())
    {
        // Get window pointer for easier usage
        Window* window = Window::get();

        // Set the window title
        window -> setTitle("Bytepusher"); 

        // Main loop
        while(!window -> isQuit())
        {
            // Update delta timings
            window -> updateDelta();

            // Poll events (updates input)
            window -> pollEvents();

            // Update the emu
            emu -> update(window -> getDelta());

            // If emu stopped, close everything
            if(!emu -> isRunning())
                window -> quit();

            // Start drawing
            window -> getRenderer() -> drawStart();

            // Draw to the screen
            emu -> draw(window);

            // End drawing
            window -> getRenderer() -> drawEnd();
        }
    }

    return EXIT_SUCCESS;
}