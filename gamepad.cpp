#include "gamepad.hpp"

#define sign(x) (x > 0) ? 1 : ((x < 0) ? -1 : 0)

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
            printf("{I}: Controller %d, XDir: %d, YDir: %d\n", event.jaxis.which, xDir[event.jaxis.which], yDir[event.jaxis.which]);
            break;
        }

        case SDL_JOYBUTTONUP:
        case SDL_JOYBUTTONDOWN:
        {
            /*if(event.jaxis.axis == 0)
                xDir[event.jaxis.which] = sign(event.jaxis.value / deadZone);
            else if(event.jaxis.axis == 1)
                yDir[event.jaxis.which] = sign(event.jaxis.value / deadZone);*/
            printf("{I}: Controller %d, button: %d, state: %d\n", event.jbutton.which, event.jbutton.button, event.jbutton.state);
            break;
        }
    }
}

int32_t GamepadManager::getXDir(int8_t who)
{
    return xDir[who];
}

int32_t GamepadManager::getYDir(int8_t who)
{
    return yDir[who];
}

int32_t GamepadManager::getdpad(int8_t who)
{
    return dpad[who];
}

uint32_t GamepadManager::getGamepadCount()
{
    return gamepadCount;
}