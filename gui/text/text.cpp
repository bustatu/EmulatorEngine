#include "text.hpp"

void Text::render()
{
    // Check if everything is set
    if(font != nullptr && renderer != nullptr)
    {
        if(result != nullptr)
            SDL_DestroyTexture(result), result = nullptr;

        SDL_Surface* textSurface = TTF_RenderText_Solid(font -> getFont(), text.c_str(), color);

        result = SDL_CreateTextureFromSurface(renderer, textSurface);

        if(result != nullptr)
            SDL_QueryTexture(result, nullptr, nullptr, &width, &height);

        SDL_FreeSurface(textSurface);
    }
}

void Text::setFont(Font* newFont)
{
    font = newFont;

    render();
}

void Text::setRenderer(SDL_Renderer* newRenderer)
{
    renderer = newRenderer;

    render();
}

SDL_Texture* Text::getTexture()
{
    return result;
}

std::string Text::getText()
{
    return text;
}

void Text::setColor(SDL_Color newColor)
{
    color = newColor;

    render();
}

void Text::setText(std::string newText)
{
    text = newText;

    render();
}

std::pair<int32_t, int32_t> Text::getSize()
{
    return std::make_pair(width, height);
}