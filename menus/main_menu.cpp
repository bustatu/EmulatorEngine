#include "main_menu.hpp"

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

    textArray[4].setFont(&menuFont);
    textArray[4].setRenderer(window -> getRenderer());
    textArray[4].setText("4. NES");
}

void MenuState::resume()
{
    // Resume
    is_running = true;
}

void MenuState::pause()
{
    // Pause
    is_running = false;
}

void MenuState::update(double dt)
{
    Window* window = stateM -> getWindow();

    if(window -> getKeyPressed(SDLK_SPACE))
    {
        if(index == 1)
        {
            CHIP8::Emu* state = new CHIP8::Emu();
            state -> load("data/chip8/roms/PONG2");
            stateM -> pushState(state);
        }
        else if(index == 2)
        {
            Bytepusher::Emu* state = new Bytepusher::Emu();
            state -> load("data/bytepusher/roms/nyan.bp");
            stateM -> pushState(state);
        }
        else if(index == 3)
        {
            Gameboy::Emu* state = new Gameboy::Emu();
            state -> loadBIOS("data/gameboy/bios/bios.gb");
            state -> loadROM("data/gameboy/roms/tetris.gb");
            stateM -> pushState(state);
        }
        else if(index == 4)
        {
            NES::Emu* state = new NES::Emu();
            state -> load("data/nes/roms/nestest.nes");
            stateM -> pushState(state);
        }
    }
    
    for(int32_t i = 1; i <= 4; i++)
        textArray[i].setColor({255, 255, 255});
    if(index >= 1 && index <= 4)
        textArray[index].setColor({255, 0, 0});

    if(window -> getKeyPressed(SDLK_s))
        index++;
    if(window -> getKeyPressed(SDLK_w))
        index--;

    index = std::max(1, index);
    index = std::min(4, index);
}

void MenuState::draw()
{
    Window* window = stateM -> getWindow();

    // TODO: Change this, I hate it

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

    rect -> x = 60;
    rect -> y = 190;
    rect -> w = textArray[4].getSize().first / 2;
    rect -> h = textArray[4].getSize().second / 2;
    SDL_RenderCopy(window -> getRenderer(), textArray[4].getTexture(), NULL, rect);

    delete rect;
}