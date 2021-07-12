#include "window.h"

///////////////////////
///     Window      ///
///////////////////////

//Initialize pointer to zero so that it can be initialized in first call to getInstance
Window *Window::instance = 0;

Window::Window()
{
    window = SDL_CreateWindow("<windowname>", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 256, 256, 0);

    renderer = new SoftwareRenderer(window);

    deltaTime = 0;
    NOW = SDL_GetPerformanceCounter();
    LAST = 0;
}

void Window::pollEvents()
{
    // Wait a bit so CPU usage is not high
    SDL_Delay(8);

    // Clear key states
    isKeyPressed.clear();
    isKeyReleased.clear();

    //Handle events on queue
    while(SDL_PollEvent(&windowEvent) != 0)
    {
        // Handle different event types
        switch(windowEvent.type)
        {
            // User requests quit
            case SDL_QUIT:
                quit();
                break;

            // Key has been pressed
            case SDL_KEYDOWN:
                // If it was not down mark it as recently pressed
                if(!isKeyDown[windowEvent.key.keysym.sym])
                    isKeyPressed[windowEvent.key.keysym.sym] = true;
                // Mark key as being held down
                isKeyDown[windowEvent.key.keysym.sym] = true;
                break;
            
            // Key has been released
            case SDL_KEYUP:
                // If it was down mark it as recently released
                if(isKeyDown[windowEvent.key.keysym.sym])
                    isKeyReleased[windowEvent.key.keysym.sym] = true;
                // Mark key as being released
                isKeyDown[windowEvent.key.keysym.sym] = false;
                break;
        }
    }
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