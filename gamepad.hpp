#ifndef GAMEPAD_H
#define GAMEPAD_H

#include <map>
#include <SDL2/SDL.h>

class GamepadManager
{
private:
    int32_t deadZone = 8080;
    std::map<int32_t, SDL_Joystick*> joysticks;
    std::map<int32_t, int8_t> xDir, yDir, dpad;
    uint32_t gamepadCount;
public:
    void init();
    void updateInput(SDL_Event event);
    int32_t getXDir(int8_t who);
    int32_t getYDir(int8_t who);
    int32_t getdpad(int8_t who);
    uint32_t getGamepadCount();
};

#endif