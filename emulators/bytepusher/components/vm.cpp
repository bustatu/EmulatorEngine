#include "vm.hpp"

namespace Bytepusher
{
    VM::VM()
    {
        // Allocate memory
        ram = new uint8_t[ram_size];
        key = new uint8_t[0x10];

        // Reset counter
        counter = 0;

        // Reset state to not running
        state = 0;
    }

    uint32_t VM::getVal(uint32_t pc, uint32_t n)
    {
        uint32_t v = 0;
        while(n--)
            v = (v << 8) + ram[pc++];
        return v;
    }

    int8_t VM::writeBuffer(uint16_t offset, uint8_t* buffer, uint32_t size)
    {
        // Safety checks
        if(offset + size - 1 > ram_size)
        {
            printf("{E}: File size is too big! Maximum is %d, got %d!\n", ram_size, offset + size - 1);
            return -1;
        }

        // memcpy the values from the buffer to the RAM
        memcpy(ram + offset, buffer, size);

        // Success
        return 0;
    }

    uint32_t VM::getState()
    {
        return state;
    }

    void VM::draw(SDL_Texture* output, SDL_Renderer* tool)
    {
        // Check memory bank related to the display
        uint8_t* frame = ram + (getVal(5, 1) << 16);

        // Draw
        SDL_SetRenderTarget(tool, output);
        for(int i = 0; i < 256; i++)
        {
            for(int j = 0; j < 256; j++)
            {
                SDL_SetRenderDrawColor(tool, palette[frame[(j << 8) | i]].r, palette[frame[(j << 8) | i]].g, palette[frame[(j << 8) | i]].b, palette[frame[(j << 8) | i]].a);
                SDL_RenderDrawPoint(tool, i, j);
            }
        }
        SDL_SetRenderTarget(tool, NULL);
    }

    void VM::update(double dt, Window* win)
    {
        // Update delta time
        counter += dt;

        // 60 Hz, forced
        while(counter >= freq && state == 1)
        {
            // Get execution location from memory
            uint32_t pc = getVal(2, 3);

            // Steps per cycle
            for(int i = 0; i < 0x10000; i++)
            {
                // memory[B] = memory[A], pc = memory[C]
                ram[getVal(pc + 3, 3)] = ram[getVal(pc, 3)];
                pc = getVal(pc + 6, 3);
            }

            // Push audio section to the sound engine
            win -> getAudio() -> push((int8_t*)(ram + (getVal(6, 2) << 8)), 256);

            // 60 Hz, forced
            counter -= freq;
        }

        // Go trough all the keys and set / unset the good one.
        for(int i = 0; i < 0x10; i++)
        {
            // If key is down mark it as such
            if(win -> getKey(keys[i])) ram[1 - i / 8] |= (1 << (i % 8));
            else ram[1 - i / 8] &= ~(1 << (i % 8));
        }
    }

    void VM::boot()
    {
        // Initialise palette
        for(int i = 0; i <= 215; i++)
        {
            // Formula for RGB format
            palette[i].b = i % 6;
            palette[i].g = ((i - palette[i].b) / 6) % 6;
            palette[i].r = ((i - palette[i].b - 6 * palette[i].g) / 36) % 6;
            palette[i].r *= 0x33;
            palette[i].g *= 0x33;
            palette[i].b *= 0x33;
            palette[i].a = 0xFF;
        }

        // The rest of the pixels are black
        for(int i = 216; i <= 255; i++)
            palette[i].r = 0x00, palette[i].g = 0x00, palette[i].b = 0x00, palette[i].a = 0xFF;

        // Start emu
        state = 1;
    }
}