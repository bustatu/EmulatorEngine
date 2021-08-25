#ifndef FONT_H
#define FONT_H

#include <iostream>
#include <SDL2/SDL_ttf.h>

class Font
{
private:
    // Font hander
    TTF_Font* font = nullptr;

    // Font size
    uint32_t size = 0;

    // Font path
    std::string path = "";

    // Loads font with specified characterstics
    void load();

public:
    // Gets the SDL font instance
    TTF_Font* getFont();

    // Set the font path
    void setPath(std::string newPath);

    // Set the font size
    void setSize(uint32_t newSize);
};

#endif