#include "main_menu.h"

void MenuState::init()
{
    Window* window = stateM -> getWindow();
    menuFont.setPath("data/fonts/Alef.ttf");
    menuFont.setSize(60);

    // Set emu text
    textArray[0].setFont(&menuFont);
    textArray[0].setRenderer(window -> getRenderer());
    textArray[0].setText("Select your emulator:");

    textArray[1].setFont(&menuFont);
    textArray[1].setRenderer(window -> getRenderer());
    textArray[1].setText("1. CHIP8");

    textArray[2].setFont(&menuFont);
    textArray[2].setRenderer(window -> getRenderer());
    textArray[2].setText("2. BytePusher");

    textArray[3].setFont(&menuFont);
    textArray[3].setRenderer(window -> getRenderer());
    textArray[3].setText("3. Gameboy");
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
            CHIP8::Emu* state = new CHIP8::Emu();
            state -> load("data/chip8/roms/WIPEOFF");
            stateM -> pushState(state);
        }
        else if(index == 2)
        {
            BYTEPUSHER_EMU* state = new BYTEPUSHER_EMU();
            state -> load("data/bytepusher/roms/audio_test.bp");
            stateM -> pushState(state);
        }
        else if(index == 3)
        {
            GAMEBOY_EMU* state = new GAMEBOY_EMU();
            state -> loadBIOS("data/gameboy/bios/bios.gb");
            stateM -> pushState(state);
        }
    }
    
    for(int32_t i = 1; i <= 3; i++)
        textArray[i].setColor({255, 255, 255});
    if(index >= 1 && index <= 3)
        textArray[index].setColor({255, 0, 0});

    if(window -> getKeyPressed(SDLK_s))
        index++;
    if(window -> getKeyPressed(SDLK_w))
        index--;

    index = std::max(1, index);
    index = std::min(3, index);
}

void MenuState::draw()
{
    Window* window = stateM -> getWindow();

    SDL_SetRenderDrawColor(window -> getRenderer(), 0x00, 0x30, 0x99, 0xFF);
    SDL_RenderClear(window -> getRenderer());

    SDL_Rect* rect = new SDL_Rect();
    rect -> x = 30;
    rect -> y = 30;
    rect -> w = textArray[0].getSize().first / 2;
    rect -> h = textArray[0].getSize().second / 2;
    SDL_RenderCopy(window -> getRenderer(), textArray[0].getTexture(), NULL, rect);

    rect -> x = 60;
    rect -> y = 70;
    rect -> w = textArray[1].getSize().first / 2;
    rect -> h = textArray[1].getSize().second / 2;
    SDL_RenderCopy(window -> getRenderer(), textArray[1].getTexture(), NULL, rect);

    rect -> x = 60;
    rect -> y = 110;
    rect -> w = textArray[2].getSize().first / 2;
    rect -> h = textArray[2].getSize().second / 2;
    SDL_RenderCopy(window -> getRenderer(), textArray[2].getTexture(), NULL, rect);

    rect -> x = 60;
    rect -> y = 150;
    rect -> w = textArray[3].getSize().first / 2;
    rect -> h = textArray[3].getSize().second / 2;
    SDL_RenderCopy(window -> getRenderer(), textArray[3].getTexture(), NULL, rect);

    delete rect;
}