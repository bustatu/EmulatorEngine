#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include <SDL2/SDL_ttf.h>
#include "../state.h"

class MenuState : public State
{
private:
    TTF_Font* menuFont = nullptr;
    SDL_Texture* message;
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