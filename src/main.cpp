#include <iostream>
#include "window/window.h"
#include "emulators/bytepusher/bytepusher.h"

int main(int argc, char* argv[])
{
    // Get ROM name
    std::string rom_name;
    std::cout << "Please write the ROM name.\nRoms can be found in the roms/bytepusher/ folder.\nROM: ";
    std::cin >> rom_name;
    if(rom_name == "") rom_name = "nyan.bp";

    // Create emu
    UnthreadedEmulator* emu = new BytePusherEmu();

    // Load the ROM
    emu -> load("roms/bytepusher/" + rom_name);

    // If emu started
    if(emu -> isRunning())
    {
        // Window init
        Window* window = new Window();
        window -> setTitle("BytePusher");

        // Event handler
        SDL_Event e;

        // Main loop
        while(!window -> isQuit())
        {
            // Update delta timings
            window -> updateDelta();

            //Handle events on queue
            while(SDL_PollEvent(&e) != 0)
            {
                // User requests quit
                if(e.type == SDL_QUIT)
                    window -> quit();

                // Do emu input
                emu -> input(e);
            }

            // Update the emu
            emu -> update(window -> getDelta());
            emu -> draw(window);

            // If emu stopped, close everything
            if(!emu -> isRunning())
                window -> quit();

            // Update the window
            SDL_UpdateWindowSurface(window -> getWin());

            // Wait a bit so CPU usage is not high
            SDL_Delay(2);
        }
    }
    return EXIT_SUCCESS;
}