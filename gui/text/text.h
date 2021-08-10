#ifndef TEXT_H
#define TEXT_H

#include "font.h"

class Text
{
private:
    Font* font = nullptr;
    SDL_Color color = {255, 255, 255};
    std::string text = "";
    SDL_Renderer* renderer = nullptr;
    SDL_Texture* result = nullptr;
    int32_t width, height;

    // Updates the resulting texture
    void render();

public:
    // Sets the currently used font to another one
    void setFont(Font* newFont);

    // Sets the renderer used for the drawing
    void setRenderer(SDL_Renderer* newRenderer);

    // Gets texture for drawing purposes
    SDL_Texture* getTexture();

    // Gets the text used in the font
    std::string getText();

    // Sets the text color
    void setColor(SDL_Color newColor);

    // Sets the text
    void setText(std::string newText);

    // Gets the size
    std::pair<int32_t, int32_t> getSize();
};


#endif