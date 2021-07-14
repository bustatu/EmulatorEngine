#ifndef ENGINE_WINDOW_H
#define ENGINE_WINDOW_H

#include <cstdlib>
#include <iostream>
#include <SDL2/SDL.h>
#include <map>
#include "renderer.h"

///////////////////////////////////////////////////////
///     Class that handles the window instance      ///
///////////////////////////////////////////////////////

class Window
{
private:
    // Window instance
    static Window *instance;

    // Window attributes
    uint64_t NOW, LAST;
    double deltaTime;
    SDL_Window* window = NULL;
    bool appQuit = false;
    SDL_Event windowEvent;
    Renderer* renderer;
 
    // Private constructor
    Window();

public:
    // Returns the window instance
    static Window *get() {
        // Create an new instance if the old one does not exist
        if(!instance)
            instance = new Window;
        return instance;
    }
    
    // Key maps
    std::map<SDL_Keycode, bool> isKeyDown, isKeyReleased, isKeyPressed;

    bool isQuit();
    void pollEvents();
    void updateDelta();
    double getDelta();
    void quit();
    void setTitle(std::string name = "EmuEngine");
    SDL_Surface* getSurface();
    Renderer* getRenderer();
    SDL_Window* getWin();
};

#endif