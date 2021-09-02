#include "gpu.hpp"

#define get_bit(who, which) (((who) >> (which)) & 1)
#define set_bit(who, which, what) who = (what) ? ((who) | (1 << (which))) : ((who) & ~(1 << (which)))

namespace Gameboy
{
    uint8_t GPU::readByte(uint16_t addr)
    {
        return regs[addr - 0xFF40];
    }

    void GPU::writeByte(uint16_t addr, uint8_t val)
    {
        if(addr != 0xFF44)
            regs[addr - 0xFF40] = val;
    }

    void GPU::setMode(uint8_t newMode)
    {
        // Sets bits in the stat register
        set_bit(STAT, 1, get_bit(newMode, 1));
        set_bit(STAT, 2, get_bit(newMode, 2));

        // Check if there needs to be an interrupt made
        bool irq = false;

        switch(newMode)
        {
            // HBlank
            case 0x00:
                irq = get_bit(STAT, 3);
                break;

            // VBlank
            case 0x01:
                irq = get_bit(STAT, 4);

                // Request VBlank interrupt
                bus -> writeByte(0xFF0F, bus -> readByte(0xFF0F) | 1);
                break;

            // OAM search
            case 0x02:
                irq = get_bit(STAT, 5);
                // TODO: clear OAM buffer
                break;

            // Pixel transfer (or drawing mode)
            case 0x03:
                // TODO: init pixel fetcher
                break;
        }

        // If we got an interrupt
        if(irq)
            // Request stat interrupt
            bus -> writeByte(0xFF0F, bus -> readByte(0xFF0F) | (1 << 1));
    }

    bool GPU::isOn()
    {
        return (LCDControl >> 7) & 1;
    }

    uint8_t GPU::getMode()
    {
        return STAT & 0b11;
    }

    void GPU::update()
    {
        // If not on, then don't do anything
        if(!isOn())
            return;

        // Update the clock
        clock++;

        // Do stuff depending on the mode
        switch(getMode())
        {
            // HBlank
            case 0x00:
            {
                // If line actually ended
                if(++pcnt == 456)
                {
                    pcnt = 0;
                    LY++;

                    // TODO: check if still inside the window

                    // We have entered VBlank
                    if(LY == 144)
                        setMode(1);
                    else
                        setMode(2);
                }
                break;
            }

            // VBlank
            case 0x01:
            {
                // Can't be true anymore
                wyTrigger = false;

                // If at the end of the line
                if(++pcnt == 456)
                {
                    pcnt = 0;
                    LY++;

                    // Go in OAM search mode
                    if (LY > 153)
                        LY = WLY = 0, setMode(2);

                    // Interrupt check
                    set_bit(STAT, 2, (LY == LYC));

                    // Request stat interrupt
                    if(get_bit(STAT, 6))
                        bus -> writeByte(0xFF0F, bus -> readByte(0xFF0F) | (1 << 1));
                }
                break;
            }

            // OAM search
            case 0x02:
            {
                if (!wyTrigger && pcnt == 0)
                    wyTrigger = (LY == WY);

                // Can only do transfers on even clock counts (they take time you know)
                if(pcnt % 2 == 0)
                {
                    uint16_t address = 0xFE00 + (pcnt / 2) * 4;

                    uint8_t YPos = bus -> readByte(address);
                    uint8_t XPos = bus -> readByte(address + 1);
                    uint8_t tileIndex = bus -> readByte(address + 2);
                    uint8_t attributes = bus -> readByte(address + 3);

                    uint8_t spriteHeight = get_bit(LCDControl, 2) ? 16 : 8;

                    if(XPos > 0 && (LY + 16) >= YPos && (LY + 16) < (YPos + spriteHeight))
                    {
                        if(spriteHeight == 16)
                        {
                            if ((LY + 16) < (YPos - 8))
                                tileIndex |= 0x01;
                            else
                                tileIndex &= 0xFE;
                        }

                        // TODO: push sprite to the OAM buffer
                    }

                    if (++pcnt == 80)
                    {
                        // Sort the buffer and start the pixel transfer
                        // TODO: sort the buffer
                        setMode(3);
                    }
                }
                break;
            }

            // Pixel transfer (or drawing mode)
            case 0x03:
            {
                // Screen will be updated
                drawFlag = true;
                
                // Update pixel counter
                pcnt++;

                // TODO: do pixel drawing to the screen after fetching from the fetcher

                // If at the end of the line
                if(++LX == 160)
                    setMode(0);
                break;
            }
        }
    }

    void GPU::draw(SDL_Texture* output, SDL_Renderer* tool)
    {
        // If we need to update
        if(drawFlag)
        {
            // Set the renderer's target
            SDL_SetRenderTarget(tool, output);

            for (int i = 0; i < 144; i++)
            {
                for (int j = 0; j < 160; j++)
                {
                    uint8_t color = 0x00;
                    switch (pixels[j][i])
                    {
                    case 0:
                        color = 0xFF;
                        break;

                    case 1:
                        color = 0xCC;
                        break;
                    
                    case 2:
                        color = 0x77;
                        break;
                    
                    default:
                        color = 0x00;
                        break;
                    }
                    SDL_SetRenderDrawColor(tool, color, color, color, 0xFF);
                    SDL_RenderDrawPoint(tool, j, i);
                }
            }

            // Reset the renderer's target
            SDL_SetRenderTarget(tool, NULL);

            // Reset the draw flag
            drawFlag = false;
        }
    }

    GPU::GPU()
    {
        // Reset everything
        clock = 0;
        cnt = 0;
    }
}
