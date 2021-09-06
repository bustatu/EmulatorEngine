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
    GamepadManager* gamepad = window -> getGamepadManager();

    if((gamepad -> getGamepadCount() && gamepad -> getButtonPressed(0)) || window -> getKeyPressed(SDLK_SPACE))
    {
        if(index == 1)
        {
            CHIP8::Emu* state = new CHIP8::Emu();
            stateM -> changeState(state);
            state -> load("data/chip8/roms/TETRIS");
        }
        else if(index == 2)
        {
            Bytepusher::Emu* state = new Bytepusher::Emu();
            stateM -> changeState(state);
            state -> load("data/bytepusher/roms/nyan.bp");
        }
        else if(index == 3)
        {
            Gameboy::Emu* state = new Gameboy::Emu();
            stateM -> changeState(state);
            state -> loadROM("data/gameboy/roms/Pokemon Red.gb");
        }
    }
    
    for(int32_t i = 1; i <= 3; i++)
        textArray[i].setColor({255, 255, 255});
    if(index >= 1 && index <= 3)
        textArray[index].setColor({255, 0, 0});

    if(gamepad -> getGamepadCount())
        index += gamepad -> getDPADPressed(2) - gamepad -> getDPADPressed(0);
    else
        index += window -> getKeyPressed(SDLK_s) - window -> getKeyPressed(SDLK_w);

    index = ((1 > index) ? 1 : index);
    index = ((3 < index) ? 3 : index);
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

    delete rect;
}

void MenuState::destroy()
{
}