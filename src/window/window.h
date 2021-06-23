#ifndef WINDOW_H
#define WINDOW_H

#include <cstdlib>
#include <iostream>
#include <SDL.h>
#include "renderer.h"

class Window
{
private:
    uint64_t NOW, LAST;
    double deltaTime;
    SDL_Window* window = NULL;
    bool appQuit = false;
    Renderer* renderer;

public:
    Window();
    bool isQuit();
    void updateDelta();
    double getDelta();
    void quit();
    void setTitle(std::string name = "EmuEngine");
    SDL_Surface* getSurface();
    Renderer* getRenderer();
    SDL_Window* getWin();
};

#endif