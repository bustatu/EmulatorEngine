#include "window.h"
#include "state.h"
#include "emulators/chip8/chip8.h"
#include "emulators/bytepusher/bytepusher.h"

// State manager
StateManager stateManager;

// State manager's window
Window* window;

int main()
{
    // Initialise everything
    stateManager.init();

    // Get window handler
    window = stateManager.getWindow();

    // TODO: make an actual selection, for now uncomment the one you want to use and change ROM path by hand and recompile.

    /*CHIP8_EMU* state = new CHIP8_EMU();
    state -> load("data/chip8/roms/PONG1");
    stateManager.pushState(state);*/

    /*BYTEPUSHER_EMU* state = new BYTEPUSHER_EMU();
    state -> load("data/bytepusher/audio_test.bp");
    stateManager.pushState(state);*/

    // Event and main loop
    SDL_Event event;
    while(stateManager.isRunning())
    {
        // Update delta time
        window -> updateDelta();

        // Reset the inputs
        window -> updateInput();

        // Poll the events and handle them
        while(SDL_PollEvent(&event))
        {
            // Event types
            switch(event.type)
            {
                // Window events
                case SDL_WINDOWEVENT:
                    switch(event.window.event)
                    {
                        // Resize
                        case SDL_WINDOWEVENT_RESIZED:
                            window -> resize(event.window.data1, event.window.data2);
                            break;
                    }
                    break;

                // Keyboard events
                case SDL_KEYDOWN:
                case SDL_KEYUP:
                    window -> updateInput(event);
                    break;

                // Quit events
                case SDL_QUIT:
                    stateManager.quit();
                    break;
            }
        }

        // Update
        stateManager.update();

        // Clear the window
        window -> clear();

        // Draw
        stateManager.draw();

        // Update the screen
        window -> update();

        // Small delay
        SDL_Delay(6);
    }
    
    return EXIT_SUCCESS;
}