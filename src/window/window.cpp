#include "window.h"

Window::Window()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("Error initializing SDL: %s\n", SDL_GetError());
        exit(0);
    }

    window = SDL_CreateWindow("EmuEngine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 256, 256, 0);

    renderer = new SoftwareRenderer(window);

    deltaTime = 0;
    NOW = SDL_GetPerformanceCounter();
    LAST = 0;
}

SDL_Window* Window::getWin()
{
    return window;
}

Renderer* Window::getRenderer()
{
    return renderer;
}

SDL_Surface* Window::getSurface()
{
    return SDL_GetWindowSurface(window);
}

void Window::updateDelta()
{
    LAST = NOW;
    NOW = SDL_GetPerformanceCounter();
    deltaTime = (double)((NOW - LAST) / (double)SDL_GetPerformanceFrequency());
}

void Window::setTitle(std::string name)
{
    SDL_SetWindowTitle(window, name.c_str());
}

double Window::getDelta()
{
    return deltaTime;
}

bool Window::isQuit()
{
    return appQuit;
}

void Window::quit()
{
    appQuit = true;
}