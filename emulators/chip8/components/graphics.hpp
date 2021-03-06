#ifndef CHIP8_GRAPHICS_H
#define CHIP8_GRAPHICS_H

#include <iostream>
#include "SDL2/SDL.h"

namespace CHIP8
{
    class Graphics
    {
    private:
        // Screen sizes
        uint16_t screen_w = 0, screen_h = 0;

        // VRAM
        uint8_t* gfx = nullptr;
        
        // Draw flag
        bool draw_flag = true;

        // Resize flag
        bool resize_flag = false;

        // State flag
        uint8_t state = 0;

        // Display colors
        SDL_Color foreground = {0x88, 0x88, 0x88, 0xFF}, background = {0x10, 0x10, 0x10, 0xFF};

    public:
        // Resize the display and clear it
        void resize(uint16_t newX, uint16_t newY);

        // Set foreground color
        void setForeground(SDL_Color color);

        // Set background color
        void setBackground(SDL_Color color);

        // Clear the display
        void clear();

        // Sets the display state (0 - normal, 1 - SCHIP)
        void setState(uint8_t newState);

        // Gets display state
        uint8_t getState();

        // Get the size of the display
        std::pair<uint8_t, uint8_t> getSize();

        // Updates a pixel at the position in the VRAM. Returns true if flag 0xF should be modified.
        uint8_t xorPixel(uint16_t pos);

        // Draw to the texture
        void draw(SDL_Texture* &target, SDL_Renderer* tool);
    };
}

#endif