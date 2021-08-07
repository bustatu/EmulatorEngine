#include "main_menu.h"

void MenuState::init()
{
    Window* window = stateM -> getWindow();
    menuFont = TTF_OpenFont("data/fonts/Alef.ttf", 30);
    message = SDL_CreateTextureFromSurface(window -> getRenderer(), TTF_RenderText_Solid(menuFont, "Helo", {255, 255, 255}));
}

void MenuState::resume()
{
    
}

void MenuState::pause()
{
    
}

void MenuState::update(double dt)
{

}

void MenuState::draw()
{
    Window* window = stateM -> getWindow();

    SDL_SetRenderDrawColor(window -> getRenderer(), 0x00, 0x22, 0x55, 0xFF);
    SDL_RenderClear(window -> getRenderer());

    SDL_RenderCopy(window -> getRenderer(), message, NULL, NULL);
}