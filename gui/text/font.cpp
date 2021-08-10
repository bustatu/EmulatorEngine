#include "font.h"

void Font::load()
{
    if(path != "" && size != 0)
        font = TTF_OpenFont(path.c_str(), size);
}

void Font::setPath(std::string newPath)
{
    // Set new path
    path = newPath;

    // Load if both parameters have been set
    load();
}

TTF_Font* Font::getFont()
{
    if(font == nullptr)
    {
        printf("\033[1;31m{E}: Attempted to get an unloaded font!\n\033[0m!\n");
        exit(0);
    }
    return font;
}

void Font::setSize(uint32_t newSize)
{
    // Set new size
    size = newSize;

    // Load if both parameters have been set
    load();
}