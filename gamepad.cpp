#include "gamepad.hpp"

#define sign(x) (x > 0) ? 1 : ((x < 0) ? -1 : 0)
#define get_bit(who, which) (((who) >> (which)) & 1)

void GamepadManager::init()
{
    // Loaded controller count
    gamepadCount = 0;

    // Check for all connected joysticks
    for(int32_t i = 0; i < SDL_NumJoysticks(); i++)
    {
        joysticks[i] = SDL_JoystickOpen(i);
        gamepadCount++;
        if(joysticks[i] == NULL)
            printf("{W}: Unable to open game controller %d! SDL Error: %s.\n", i, SDL_GetError()), gamepadCount--;
    }
    printf("{I}: Loaded %d game controllers!\n", gamepadCount);
}

void GamepadManager::updateInput(SDL_Event event)
{
    switch(event.type)
    {
        case SDL_JOYAXISMOTION:
        {
            if(event.jaxis.axis == 0)
                xDir[event.jaxis.which] = sign(event.jaxis.value / deadZone);
            else if(event.jaxis.axis == 1)
                yDir[event.jaxis.which] = sign(event.jaxis.value / deadZone);
            //printf("{I}: Controller %d, XDir: %d, YDir: %d\n", event.jaxis.which, xDir[event.jaxis.which], yDir[event.jaxis.which]);
            break;
        }

        case SDL_JOYBUTTONUP:
        case SDL_JOYBUTTONDOWN:
        {
            buttons[event.jbutton.button] = event.jbutton.state;
            //printf("{I}: Controller %d, button: %d, state: %d\n", event.jbutton.which, event.jbutton.button, event.jbutton.state);
            break;
        }

        case SDL_JOYHATMOTION:
        {
            dpad = event.jhat.value;
            //printf("{I}: Controller %d, hat: %d, value: %d\n", event.jhat.which, event.jhat.hat, event.jhat.value);
            break;
        }
    }
}

void GamepadManager::updateInput()
{
    lastdpad = dpad;

    for(auto i : buttons)
        lastButtons[i.first] = i.second;
}

int32_t GamepadManager::getXDir(int8_t who)
{
    return xDir[who];
}

int32_t GamepadManager::getYDir(int8_t who)
{
    return yDir[who];
}

int32_t GamepadManager::getDPAD(int8_t who)
{
    return get_bit(dpad, who);
}

int32_t GamepadManager::getDPADPressed(int8_t who)
{
    return !get_bit(dpad, who) && get_bit(lastdpad, who);
}

int32_t GamepadManager::getDPADReleased(int8_t who)
{
    return get_bit(dpad, who) && !get_bit(lastdpad, who);
}

int32_t GamepadManager::getButton(int8_t who)
{
    return buttons[who];
}

int32_t GamepadManager::getButtonPressed(int8_t who)
{
    return buttons[who] && !lastButtons[who];
}

int32_t GamepadManager::getButtonReleased(int8_t who)
{
    return !buttons[who] && lastButtons[who];
}

uint32_t GamepadManager::getGamepadCount()
{
    return gamepadCount;
}