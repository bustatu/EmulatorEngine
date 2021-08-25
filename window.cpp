#include "window.hpp"

Window::Window()
{
    // Init SDL
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
    {
        printf("\033[1;31m{E}: An error occured while initialising SDL! %s\n\033[0m", SDL_GetError());
        exit(0);
    }
    if(TTF_Init() != 0)
    {
        printf("\033[1;31m{E}: An error occured while initialising SDL_ttf! %s\n\033[0m", SDL_GetError());
        exit(0);
    }

    // Create audio
    audio = new Audio();

    // Create window with default sizes and title
    window = SDL_CreateWindow("<title>", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 960, 540, SDL_WINDOW_RESIZABLE);
    size_x = 960;   size_y = 540;

    if(window == NULL)
    {
        printf("\033[1;31m{E}: An error occured while creating the window! %s\n\033[0m", SDL_GetError());
        exit(0);
    }

    // Create renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if(renderer == NULL)
    {
        printf("\033[1;31m{E}: An error occured while creating the renderer! %s\n\033[0m", SDL_GetError());
        exit(0);
    }

    // Start timing
    now = SDL_GetPerformanceCounter();

    is_open = true;
}

void Window::updateInput(SDL_Event event)
{
    if(event.type == SDL_KEYDOWN)
    {
        state[event.key.keysym.sym] = true;
        pressed[event.key.keysym.sym] = true;
    }
    else if(event.type == SDL_KEYUP)
    {
        state[event.key.keysym.sym] = false;
        released[event.key.keysym.sym] = true;
    }
}

Audio* Window::getAudio()
{
    return audio;
}

bool Window::getKey(SDL_Keycode key)
{
    return state[key];
}

bool Window::getKeyPressed(SDL_Keycode key)
{
    return pressed[key];
}

bool Window::getKeyReleased(SDL_Keycode key)
{
    return released[key];
}

void Window::updateInput()
{
    released.clear();
    pressed.clear();
}

void Window::setTitle(std::string what)
{
    SDL_SetWindowTitle(window, what.c_str());
}

void Window::close()
{
    is_open = false;

    // Destroy everything
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
}

void Window::resize(uint32_t newX, uint32_t newY)
{
    SDL_SetWindowSize(window, newX, newY);
    size_x = newX;
    size_y = newY;
}

std::pair<uint32_t, uint32_t> Window::getSize()
{
    return std::make_pair(size_x, size_y);
}

SDL_Renderer* Window::getRenderer()
{
    return renderer;
}

bool Window::isOpen()
{
    return is_open;
}

void Window::clear()
{
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(renderer);
}

void Window::updateDelta()
{
    last = now;
    now = SDL_GetPerformanceCounter();
    delta_time = ((now - last) * 1.0) / SDL_GetPerformanceFrequency();
}

double Window::getDelta()
{
    return delta_time;
}

void Window::update()
{
    SDL_RenderPresent(renderer);
}