#include "gpu.hpp"

#define get_bit(who, which) (((who) >> (which)) & 1)
#define set_bit(who, which, what) who = (what) ? ((who) | (1 << (which))) : ((who) & ~(1 << (which)))

namespace Gameboy
{
    uint8_t GPU::get_colour(uint8_t colourNum, uint8_t palette)
    {
        return (get_bit(palette, 2 * colourNum + 1) << 1) | get_bit(palette, 2 * colourNum);
    }

    void GPU::tickSpriteFetcher()
    {
        // Do stuff only on even ticks
        if (++spf_ticks % 2 != 0) return;

        // Do stuff depending on the sprite fetcher's state
        switch(pf_state2)
        {
            // Fetch tile no.
            case 0x00:
            {
                sTileNo = cur_sprite.tileIndex;
                pf_state2 = 0x01;
                break;
            }
            
            // Fetch data low
            case 0x01:
            {
                uint16_t tileMap = 0x8000;
                uint16_t offset2 = 0;

                if(get_bit(cur_sprite.attr, 6))
                {
                    offset2 = (get_bit(LCDControl, 2) ? 30 : 14) - 2 * (LY - (cur_sprite.y - 16));
                }
                else
                {
                    offset2 = 2 * (LY - (cur_sprite.y - 16));
                }

                addr = tileMap + sTileNo * 16 + offset2;
                sTileLo = bus -> readByte(addr);

                pf_state2 = 0x02;
                break;
            }

            // Fetch data high
            case 0x02:
            {
                sTileHi = bus -> readByte(addr + 1);
                pf_state2 = 0x03;
                break;
            }

            // Push to FIFO
            case 0x03:
            {
                for(int bit = 7, i = 0; bit >= 0; bit--, i++)
                {
                    if ((cur_sprite.x + i) >= 8)
                    {
                        int effectiveBit = get_bit(cur_sprite.attr, 5) ? 7 - bit : bit;
                        uint8_t colourNum = (get_bit(sTileHi, effectiveBit) << 1) | get_bit(sTileLo, effectiveBit);
                        Pixel pix{get_colour(colourNum, get_bit(cur_sprite.attr, 4) ? OBP1 : OBP0), colourNum == 0, false, (bool)get_bit(cur_sprite.attr, 7)};

                        if(i >= spFIFO.getSize())
                        {
                            spFIFO.push(pix);
                        }
                        else
                        {
                            if(spFIFO[i].transparent)
                            {
                                spFIFO[i] = pix;
                            }
                        }
                    }
                }

                fetchingSprites = false;
                pf_state1 = 0x00;
                break;
            }
        }
    }

    void GPU::tickBackgroundFetcher()
    {
        // Do stuff only on even ticks
        if (++bgf_ticks % 2 != 0) return;

        switch (pf_state1)
        {
            // Fetch tile no.
            case 0x00:
            {
                uint16_t tileMap;
                uint16_t xOffset;
                uint16_t yOffset;

                if (fetchingWindow)
                {
                    tileMap = get_bit(LCDControl, 6) ? 0x9C00 : 0x9800;
                    xOffset = fetcherX;
                    yOffset = (WLY / 8) * 32;
                }
                else
                {
                    tileMap = get_bit(LCDControl, 3) ? 0x9C00 : 0x9800;
                    xOffset = (fetcherX + (SCX / 8)) & 0x1F;
                    yOffset = 32 * (((LY + SCY) & 0xFF) / 8);
                }

                addr = tileMap + ((xOffset + yOffset) & 0x3FF);
                tileNo = bus -> readByte(addr);

                pf_state1 = 0x01;
                break;
            }

            // Fetch data low
            case 0x01:
            {
                bool useSigned = (get_bit(LCDControl, 4) == 0);
                uint16_t tileData = useSigned ? 0x9000 : 0x8000;
                uint16_t offset1 = (useSigned ? int8_t(tileNo) : tileNo) * 16;
                uint16_t offset2 = 2 * (fetchingWindow ? (WLY % 8) : ((LY + SCY) % 8));

                addr = tileData + offset1 + offset2;
                tileLo = bus -> readByte(addr);

                pf_state1 = 0x02;
                break;
            }

            // Fetch data high
            case 0x02:
            {
                tileHi = bus -> readByte(addr + 1);
                pf_state1 = 0x03;
                break;
            }

            // Push to FIFO
            case 0x03:
            {
                if (bgFIFO.isEmpty())
                {
                    for (int bit = 7; bit >= 0; --bit)
                    {
                        uint8_t colourNum = (get_bit(tileHi, bit) << 1) | get_bit(tileLo, bit);
                        bgFIFO.push(Pixel{get_colour(colourNum, BGP), colourNum == 0, (bool)get_bit(LCDControl, 0), false});
                    }

                    fetcherX++;
                    pf_state1 = 0x00;
                }
                break;
            }
        }
    }

    Pixel GPU::mixPixels(Pixel bg, Pixel sp)
    {
        if(!sp.transparent)
        {
            if(sp.bgPriority && !bg.transparent)
                return bg;
            return sp;
        }
        return bg;
    }

    void GPU::tickFetcher()
    {
        if (!fetchingSprites && get_bit(LCDControl, 1) && oamBuffer.size() > 0)
        {
            Sprite s = oamBuffer.front();

            if(s.x <= LX + 8)
            {
                cur_sprite = s;
                pf_state2 = 0x00;
                spf_ticks = 0;
                fetchingSprites = true;
                oamBuffer.pop_front();
            }
        }

        if (fetchingSprites)
        {
            tickSpriteFetcher();
            return;
        }

        if (!fetchingWindow && get_bit(LCDControl, 5) && wyTrigger && LX >= (WX - 7))
        {
            fetcherX = 0;
            pf_state1 = 0x00;
            fetchingWindow = true;
            bgFIFO.clear();
        }

        tickBackgroundFetcher();
    }

    void PixelFIFO::push(Pixel pix)
    {
        buffer[head] = pix;
        head = (head + 1) % 8;
        size++;
    }

    void PixelFIFO::clear()
    {
        head = tail = size = 0;
    }

    bool PixelFIFO::isEmpty()
    {
        return size == 0;
    }

    Pixel PixelFIFO::pop()
    {
        Pixel pix = buffer[tail];
        tail = (tail + 1) % 8;
        size--;
        return pix;
    }

    int32_t PixelFIFO::getSize()
    {
        return size;
    }

    bool GPU::isPFReady()
    {
        bool ret = !bgFIFO.isEmpty() && !fetchingSprites;

        if (ret && LX < 0)
        {
            bgFIFO.pop();
            LX++;
            ret = false;
        }

        return ret;
    }

    uint8_t GPU::readByte(uint16_t addr)
    {
        return regs[addr - 0xFF40];
    }

    void GPU::writeByte(uint16_t addr, uint8_t val)
    {
        // Do DMA transfer
        if(addr == 0xFF46)
        {
            if(val >= 0x00 && val <= 0xDF)
            {
                DMA = val;
                for(int32_t i = 0x00; i < 0xA0; i++)
                    bus -> writeByte(0xFE00 | i, bus -> readByte((DMA << 8) | i));
            }
        }
        else if(addr != 0xFF44)
            regs[addr - 0xFF40] = val;
    }

    void GPU::setMode(uint8_t newMode)
    {
        // Sets bits in the stat register
        STAT &= 0b11111100;
        STAT |= newMode;

        // Check if there needs to be an interrupt made
        bool irq = false;

        switch(newMode)
        {
            // HBlank
            case 0x00:
            {
                irq = get_bit(STAT, 3);
                break;
            }

            // VBlank
            case 0x01:
            {
                irq = get_bit(STAT, 4);

                // Request VBlank interrupt
                bus -> writeByte(0xFF0F, bus -> readByte(0xFF0F) | 1);
                break;
            }

            // OAM search
            case 0x02:
            {
                irq = get_bit(STAT, 5);
                oamBuffer.clear();
                break;
            }

            // Pixel transfer (or drawing mode)
            case 0x03:
            {
                // Initialise pixel fetcher
                fetcherX = 0;
                LX = -(SCX % 8);
                pf_state1 = 0x00;
                bgf_ticks = 0;
                fetchingSprites = false;
                fetchingWindow = false;
                bgFIFO.clear();
                spFIFO.clear();
                break;
            }
        }

        // If we got an interrupt
        if(irq)
            // Request stat interrupt
            bus -> writeByte(0xFF0F, bus -> readByte(0xFF0F) | (1 << 1));
    }

    uint8_t GPU::getMode()
    {
        return STAT & 0b11;
    }

    void GPU::update()
    {
        // If not on
        if(!get_bit(LCDControl, 7))
        {
            if(!resetFlag)
            {
                setMode(0x00);
                LY = 0;
                pcnt = 0;
                memset(pixels, 0, 160 * 144);
                resetFlag = true;
            }
            return;
        }
        else if(resetFlag)
        {
            setMode(0x02);
            resetFlag = false;
        }

        // Do stuff depending on the mode
        switch(getMode())
        {
            // HBlank
            case 0x00:
            {
                // If line actually ended
                if(++pcnt == 456)
                {
                    // Reset the pixel counter and go to next line
                    pcnt = 0;
                    LY++;

                    // If we are currently still fetching the window pixels
                    // also update the window line
                    if(fetchingWindow)
                        WLY++;

                    // We have entered VBlank
                    if(LY >= 144)
                        setMode(0x01);
                    else
                        setMode(0x02);

                    // Interrupt check
                    set_bit(STAT, 2, (LY == LYC));

                    // Request stat interrupt
                    if(LY == LYC && get_bit(STAT, 6))
                        bus -> writeByte(0xFF0F, bus -> readByte(0xFF0F) | (1 << 1));
                }
                break;
            }

            // VBlank
            case 0x01:
            {
                // Screen will be updated
                drawFlag = true;

                // Can't be true anymore
                wyTrigger = false;

                // If at the end of the line
                if(++pcnt == 456)
                {
                    // Go to the next one
                    pcnt = 0;
                    LY++;

                    // Reset and go in OAM search mode
                    if (LY == 154)
                        LY = WLY = 0, setMode(0x02);

                    // Interrupt check
                    set_bit(STAT, 2, (LY == LYC));

                    // Request stat interrupt
                    if(LY == LYC && get_bit(STAT, 6))
                        bus -> writeByte(0xFF0F, bus -> readByte(0xFF0F) | (1 << 1));
                }
                break;
            }

            // OAM search
            case 0x02:
            {
                // Check if we are on window Y
                if (!wyTrigger && pcnt == 0)
                    wyTrigger = (LY == WY);

                // Can only do transfers on even clock counts (they take time you know)
                if(pcnt % 2 == 0)
                {
                    // Get sprite adress
                    uint16_t address = 0xFE00 + (pcnt / 2) * 4;

                    // Fetch sprite info
                    uint8_t YPos = bus -> readByte(address);
                    uint8_t XPos = bus -> readByte(address + 1);
                    uint8_t tileIndex = bus -> readByte(address + 2);
                    uint8_t attributes = bus -> readByte(address + 3);
                    uint8_t spriteHeight = get_bit(LCDControl, 2) ? 16 : 8;

                    // If sprite can be rendered
                    if(XPos > 0 && (LY + 16) >= YPos && (LY + 16) < (YPos + spriteHeight) && oamBuffer.size() < 10)
                    {
                        // If the spriite's height is 16
                        if(spriteHeight == 16)
                        {
                            if ((LY + 16) < (YPos - 8))
                                tileIndex |= 0x01;
                            else
                                tileIndex &= 0xFE;
                        }

                        // Push sprite to OAM buffer
                        oamBuffer.push_back(Sprite{XPos, YPos, tileIndex, attributes, address});
                    }
                }

                // End of OAM scan
                if (++pcnt == 80)
                {
                    // Sort the buffer and start the pixel transfer
                    for(uint32_t i = 0; i < oamBuffer.size(); i++)
                    {
                        for(uint32_t j = i + 1; j < oamBuffer.size(); j++)
                            if(oamBuffer[j] < oamBuffer[i])
                                std::swap(oamBuffer[i], oamBuffer[j]);
                    }
                    setMode(0x03);
                }
                break;
            }

            // Pixel transfer (or drawing mode)
            case 0x03:
            {
                // Update pixel counter and fetcher
                pcnt++;
                tickFetcher();
                
                // If finished processing the pixels
                if(isPFReady())
                {
                    // We can fetch the pixel
                    Pixel pix = bgFIFO.pop();

                    // If background was disabled
                    if(!pix.bgEnable)
                    {
                        // Get the sprite pixel
                        if(!spFIFO.isEmpty())
                            pix = spFIFO.pop();
                        // It's transparent
                        else
                            pix.colour = 0;
                    }
                    // Else mix with the sprite pixel if it exists
                    else if(!spFIFO.isEmpty())
                        pix = mixPixels(pix, spFIFO.pop());

                    // Set the pixel
                    pixels[LY * 160 + LX] = pix.colour;

                    // If at the end of the line
                    if(++LX == 160)
                        setMode(0x00);
                }

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
                    switch (pixels[i * 160 + j])
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
        cnt = 0;

        SCX = 0;
        WLY = 0;
    }
}
