#include "main_menu.h"

void MenuState::init()
{
    Window* window = stateM -> getWindow();
    menuFont.setPath("data/fonts/Alef.ttf");
    menuFont.setSize(60);

    // Set emu text
    chooseEmu.setFont(&menuFont);
    chooseEmu.setRenderer(window -> getRenderer());
    chooseEmu.setText("Select your emulator:");

    chipText.setFont(&menuFont);
    chipText.setRenderer(window -> getRenderer());
    chipText.setText("1. CHIP8");

    bytepusherText.setFont(&menuFont);
    bytepusherText.setRenderer(window -> getRenderer());
    bytepusherText.setText("2. BytePusher");
}

void MenuState::resume()
{
    
}

void MenuState::pause()
{
    
}

void MenuState::update(double dt)
{
    Window* window = stateM -> getWindow();

    if(window -> getKeyPressed(SDLK_SPACE))
    {
        if(index == 1)
        {
            CHIP8_EMU* state = new CHIP8_EMU();
            state -> load("data/chip8/roms/KEYBOARD_TEST");
            stateM -> pushState(state);
        }
        else if(index == 2)
        {
            BYTEPUSHER_EMU* state = new BYTEPUSHER_EMU();
            state -> load("data/bytepusher/roms/audio_test.bp");
            stateM -> pushState(state);
        }
    }

    switch(index)
    {
        case 1:
            chipText.setColor({255, 0, 0});
            bytepusherText.setColor({255, 255, 255});
            break;
        case 2:
            chipText.setColor({255, 255, 255});
            bytepusherText.setColor({255, 0, 0});
            break;
        default:
            chipText.setColor({255, 255, 255});
            bytepusherText.setColor({255, 255, 255});
            break;
    }

    if(window -> getKeyPressed(SDLK_s))
        index++;
    if(window -> getKeyPressed(SDLK_w))
        index--;

    index = std::max(1, index);
    index = std::min(2, index);
}

void MenuState::draw()
{
    Window* window = stateM -> getWindow();

    SDL_SetRenderDrawColor(window -> getRenderer(), 0x00, 0x22, 0x55, 0xFF);
    SDL_RenderClear(window -> getRenderer());

    SDL_Rect* rect = new SDL_Rect();
    rect -> x = 30;
    rect -> y = 30;
    rect -> w = chooseEmu.getSize().first / 2;
    rect -> h = chooseEmu.getSize().second / 2;
    SDL_RenderCopy(window -> getRenderer(), chooseEmu.getTexture(), NULL, rect);

    rect -> x = 60;
    rect -> y = 70;
    rect -> w = chipText.getSize().first / 2;
    rect -> h = chipText.getSize().second / 2;
    SDL_RenderCopy(window -> getRenderer(), chipText.getTexture(), NULL, rect);

    rect -> x = 60;
    rect -> y = 110;
    rect -> w = bytepusherText.getSize().first / 2;
    rect -> h = bytepusherText.getSize().second / 2;
    SDL_RenderCopy(window -> getRenderer(), bytepusherText.getTexture(), NULL, rect);
}