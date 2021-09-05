#ifndef GAMEBOY_GPU_H
#define GAMEBOY_GPU_H

#include <deque>
#include <algorithm>
#include <SDL2/SDL.h>
#include "../bus/bus.hpp"

/*  Thanks to these 2 for letting me understand what is going on:
      (using these I also made the pixelFIFO and pixelFetcher)
          https://hacktixme.ga/GBEDG/ppu/#sprite-fetching
                https://github.com/marethyu/noufu/                 */

namespace Gameboy
{
    class Bus;

    struct Pixel
    {
        uint8_t colour;
        bool transparent;
        bool bgEnable;
        bool bgPriority;
    };

    struct Sprite
    {
        uint8_t x;
        uint8_t y;
        uint8_t tileIndex;
        uint8_t attr;
        uint16_t oamAddr;

        bool operator<(const Sprite &other)
        {
            if (x == other.x)
                return oamAddr < other.oamAddr;
            return x < other.x;
        }
    };

    class PixelFIFO
    {
    private:
        Pixel buffer[8];
        int head, tail, size;

    public:
        void push(Pixel pix);
        void clear();
        bool isEmpty();
        Pixel pop();
        int32_t getSize();

        Pixel operator[](int32_t index) const
        {
            return buffer[(tail + index) % 8];
        }

        Pixel &operator[](int32_t index)
        {
            return buffer[(tail + index) % 8];
        }
    };
    
    class GPU
    {
    private:
        // Framebuffer for an easier life
        uint8_t pixels[161 * 144];

        // Registers (FF40 - FF4B)
        uint8_t regs[0xC] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        uint8_t &LCDControl = regs[0x0];
        uint8_t &STAT = regs[0x1];
        uint8_t &SCY = regs[0x2];
        uint8_t &SCX = regs[0x3];
        uint8_t &LY = regs[0x4];
        uint8_t &LYC = regs[0x5];
        uint8_t &DMA = regs[0x6];
        uint8_t &BGP = regs[0x7];
        uint8_t &OBP0 = regs[0x8];
        uint8_t &OBP1 = regs[0x9];
        uint8_t &WY = regs[0xA];
        uint8_t &WX = regs[0xB];

        // LY but for the X axis
        uint8_t LX = 0;

        // OAM buffer
        std::deque<Sprite> oamBuffer;

        // Window line counter
        uint32_t WLY = 0;

        // True when LY = WY
        bool wyTrigger = false;

        // Frame counter
        uint32_t cnt;

        // Pixel counter
        uint32_t pcnt = 0;

        // PIXEL FETCHER REGISTERS
        int fetcherX;
        int pf_state1;
        int pf_state2;
        int bgf_ticks;
        int spf_ticks;
        bool fetchingSprites;
        bool fetchingWindow;
        Sprite cur_sprite;
        uint16_t addr;
        uint8_t tileNo;
        uint8_t tileLo;
        uint8_t tileHi;
        uint8_t sTileNo;
        uint8_t sTileLo;
        uint8_t sTileHi;

        PixelFIFO bgFIFO;
        PixelFIFO spFIFO;

        // Update the fetchers
        void tickFetcher();
        void tickBackgroundFetcher();
        void tickSpriteFetcher();

        // Gets a color from a palette
        uint8_t get_colour(uint8_t colourNum, uint8_t palette);

        // Mix 2 pixels
        Pixel mixPixels(Pixel bg, Pixel sp);

        // Changes display mode to a new value
        void setMode(uint8_t newMode);

        // Chenges the scanline
        void changeScanline();

        // Draw flag controls if the screen should be updated
        bool drawFlag = true;

        // Reset flag
        bool resetFlag = false;

        // Gets current display mode
        uint8_t getMode();

        // Finds out if pixel fetcher is ready
        bool isPFReady();

    public:
        // Constructor
        GPU();

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