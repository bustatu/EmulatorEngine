#ifndef GAMEBOY_GPU_H
#define GAMEBOY_GPU_H

#include <SDL2/SDL.h>
#include "../bus/bus.hpp"

/*  Thanks to these 2 for letting me understand what is going on:
      (using these I also made the pixelFIFO and pixelFetcher)
          https://hacktixme.ga/GBEDG/ppu/#sprite-fetching
                https://github.com/marethyu/noufu/                 */

namespace Gameboy
{
    class Bus;
    
    class GPU
    {
    private:
        // Framebuffer for an easier life
        uint8_t pixels[160][144];

        // GPU internal clock
        uint32_t clock;

        // Registers (FF40 - FF4B)
        uint8_t regs[0xC] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        uint8_t &LCDControl = regs[0x0];
        uint8_t &STAT = regs[0x1];
        uint8_t &SCX = regs[0x2];
        uint8_t &SCY = regs[0x3];
        uint8_t &LY = regs[0x4];
        uint8_t &LYC = regs[0x5];
        uint8_t &DMA = regs[0x6];
        uint8_t &BGP = regs[0x7];
        uint8_t &OBP0 = regs[0x8];
        uint8_t &OBP1 = regs[0x9];
        uint8_t &WY = regs[0xA];
        uint8_t &WX = regs[0xB];

        // LY but for the X axis
        uint8_t LX;

        // Window line counter
        uint32_t WLY;

        // True when LY = WY
        bool wyTrigger;

        // Frame counter
        uint32_t cnt;

        // Pixel counter
        uint32_t pcnt;

        // Changes display mode to a new value
        void setMode(uint8_t newMode);

        // Chenges the scanline
        void changeScanline();

        // Draw flag controls if the screen should be updated
        bool drawFlag = true;

        // Gets current display mode
        uint8_t getMode();

    public:
        // Constructor
        GPU();

        // Gets if the GPU is currently on
        bool isOn();

        // Update
        void update();

        // Read byte
        uint8_t readByte(uint16_t addr);

        // Write byte
        void writeByte(uint16_t addr, uint8_t val);

        // Draw to the output texture
        void draw(SDL_Texture* output, SDL_Renderer* tool);

        // Bus for reading data
        Bus* bus;
    };
}

#endif