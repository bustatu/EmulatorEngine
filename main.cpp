#define SDL_MAIN_HANDLED
#include "window.h"
#include "state.h"
#include "menus/main_menu.h"

// State manager
StateManager stateManager;

// State manager's window
Window* window;

int main()
{
    // Initialise everything
    stateManager.init();

    // Get window handler for easier usage
    window = stateManager.getWindow();

    // Create menu state
    State* menuState = new MenuState();

    // Push the menu state
    stateManager.pushState(menuState);

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