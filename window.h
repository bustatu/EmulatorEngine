#ifndef WINDOW_H
#define WINDOW_H

#include "audio.h"
#include <map>
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

class Window
{
private:
    // SDL default window
    SDL_Window* window = nullptr;

    // SDL renderer
    SDL_Renderer* renderer;

    // Window state flag
    bool is_open = false;

    // Delta timings
    uint64_t now, last;
    double delta_time = 0;

    // Window sizes
    uint32_t size_x, size_y;

    // Keys
    std::map<SDL_Keycode, bool> pressed, released, state;

    // Audio controller
    Audio* audio;

public:
    // Constructor
    Window();

    // Gets main renderer
    SDL_Renderer* getRenderer();

    // Sets the title of the window
    void setTitle(std::string what);

    // Clears the contents of the window
    void clear();

    // Updates the contents of the window
    void update();

    // Get window size
    std::pair<uint32_t, uint32_t> getSize();

    // Get audio
    Audio* getAudio();

    // Gets window state
    bool isOpen();

    // Resizes window
    void resize(uint32_t newX, uint32_t newY);

    // Gets key status
    bool getKey(SDL_Keycode key);
    
    // Gets key pressed status
    bool getKeyPressed(SDL_Keycode key);

    // Gets key released status
    bool getKeyReleased(SDL_Keycode key);

    // Updates input from event
    void updateInput(SDL_Event event);

    // Resets inputs
    void updateInput();

    // Update delta time
    void updateDelta();

    // Get delta time
    double getDelta();
    
    // Closes the window
    void close();
};

#endif