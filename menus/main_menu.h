#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include <SDL2/SDL_ttf.h>
#include "../state.h"
#include "../gui/font.h"
#include "../gui/text.h"
#include "../emulators/chip8/chip8.h"
#include "../emulators/bytepusher/bytepusher.h"

class MenuState : public State
{
private:
    Font menuFont;
    Text chooseEmu, chipText, bytepusherText;
    int32_t index = 0;
public:
    // Init the menu
    virtual void init();

    // Resume the menu
    virtual void resume();

    // Stop the menu
    virtual void pause();

    // Main update function
    virtual void update(double dt);

    // Main draw function
    virtual void draw();
};

#endif