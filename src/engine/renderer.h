#ifndef RENDERER_H
#define RENDERER_H

#include <iostream>
#include <SDL.h>

class Renderer
{
protected:
    SDL_Window* parentWin = NULL;
public:
    Renderer(SDL_Window* win){
        parentWin = win;
    }
    virtual void render(SDL_Surface* surf) = 0;
    virtual void drawStart() = 0;
    virtual void drawEnd() = 0;
};

class SoftwareRenderer : public Renderer
{
public:
    void render(SDL_Surface* surf) override;
    void drawStart() override;
    void drawEnd() override;
    SoftwareRenderer(SDL_Window* win) : Renderer(win) {};
};

class AcceleratedRenderer : public Renderer
{
private:
    SDL_Renderer* renderer = nullptr;
public:
    void render(SDL_Surface* surf) override;
    void drawStart() override;
    void drawEnd() override;
    AcceleratedRenderer(SDL_Window* win) : Renderer(win) {
        renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_TEXTUREACCESS_STREAMING);
        if(renderer == nullptr)
            printf("{E}: SDL_CreateRenderer Error: %s\n", SDL_GetError());
    }   
};

#endif