#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include <SDL2/SDL_ttf.h>
#include "../state.hpp"
#include "../gui/gui.hpp"
#include "../emulators/emulators.hpp"

class MenuState : public State
{
private:
    Font menuFont;
    Text textArray[5];
    int32_t index = 0;
public:
    // Init the menu
    void init();

    // Resume the menu
    void resume();

    // Stop the menu
    void pause();

    // Main update function
    void update(double dt);

    // Main draw function
    void draw();

    // Destroy the menu after execution ended
    void destroy();
};

#endif