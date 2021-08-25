#include "graphics.hpp"

namespace CHIP8
{
    void Graphics::resize(uint16_t newX, uint16_t newY)
    {
        // Update size
        screen_w = newX;
        screen_h = newY;

        // Delete VRAM if existing
        if(gfx != nullptr)
            delete gfx;
        gfx = new uint8_t[screen_w * screen_h];
        clear();

        // Update the screen
        draw_flag = true;
        resize_flag = true;
    }

    void Graphics::setForegroundColor(SDL_Color color)
    {
        foreground = color;
    }

    void Graphics::setBackgroundColor(SDL_Color color)
    {
        background = color;
    }

    std::pair<uint8_t, uint8_t> Graphics::getSize()
    {
        return std::make_pair(screen_w, screen_h);
    }

    uint8_t Graphics::xorPixel(uint16_t pos)
    {
        // Update pixel only in-bounds
        if(pos >= 0 && pos < screen_w * screen_h)
        {
            // Set draw flag
            draw_flag = true;

            // Update the pixel
            if(gfx[pos] == 1)
            {
                gfx[pos] ^= 1;
                return 1;
            }
            else
                gfx[pos] ^= 1;
        }
        return 0;
    }

    void Graphics::clear()
    {
        // Set the VRAM to 0
        memset(gfx, 0, screen_w * screen_h);
    }

    void Graphics::setState(uint8_t newState)
    {
        // Update the state
        state = newState;
    }

    uint8_t Graphics::getState()
    {
        // Return the GFX state
        return state;
    }

    void Graphics::draw(SDL_Texture *&target, SDL_Renderer* tool)
    {
        // If the resize flag has been set, resize the output texture
        if(resize_flag)
        {
            if(target != nullptr)
                SDL_DestroyTexture(target);
            target = SDL_CreateTexture(tool, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, screen_w, screen_h);
            
            // Reset the flag
            resize_flag = false;
        }

        // Do the drawing only if the screen actually needs to be updated
        if(draw_flag)
        {
            SDL_SetRenderTarget(tool, target);
            for(uint32_t x = 0; x < screen_w; x++)
            {
                for(uint32_t y = 0; y < screen_h; y++)
                {
                    if(gfx[y * screen_w + x])
                        SDL_SetRenderDrawColor(tool, foreground.r, foreground.g, foreground.b, foreground.a);
                    else
                        SDL_SetRenderDrawColor(tool, background.r, background.g, background.b, background.a);
                    SDL_RenderDrawPoint(tool, x, y);
                }
            }
            SDL_SetRenderTarget(tool, NULL);

            // Reset the flag
            draw_flag = false;
        }
    }
}