#ifndef GAMEBOY_GPU_H
#define GAMEBOY_GPU_H

#include <SDL2/SDL.h>
#include "../bus/bus.hpp"

/* Thanks to this for letting me understand what is going on:
   https://github.com/paramsingh/gameboi */

namespace Gameboy
{
    class GPU
    {
    private:
        // Framebuffer for an easier life
        uint8_t pixels[160][144];

        // GPU internal clock
        uint32_t clock;

        // Status register adress
        const uint16_t statReg = 0xFF41;

        // Line register adress
        const uint16_t lineReg = 0xFF44;

        // LCD control adress
        const uint16_t LCDControl = 0xFF40;

        // Frame counter
        uint32_t cnt;

        // Bus for reading data
        Bus* bus;

        // Changes display mode to a new value
        void setMode(uint8_t newMode);

        // Chenges the scanline
        void changeScanline();

        // Draw flag controls if the screen should be updated
        bool drawFlag = true;

        // Gets current display mode
        uint8_t getMode();

        // Render the tiles for displaying
        void renderTiles();

        // Reads a word from the bus
        uint16_t readWord(uint16_t addr);

        // Gets color depending on the palette
        int32_t getColor(int32_t id, uint16_t palette);

    public:
        // Constructor
        GPU();

        // Gets if the GPU is currently on
        bool isOn();

        // Update
        void update();

        // Draw to the output texture
        void draw(SDL_Texture* output, SDL_Renderer* tool);

        // Attach a bus for reading data
        void attachBus(Bus* newBus);
    };
}

#endif