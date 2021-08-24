#include "gpu.h"

namespace Gameboy
{
    void GPU::attachBus(Bus* newBus)
    {
        // Update the bus
        bus = newBus;

        // Set the display mode to 1
        setMode(1);

        // Reset values in the memory
        bus -> writeByte(lineReg, 0);
    }

    void GPU::setMode(uint8_t newMode)
    {
        // Sets bits in the stat register
        if(newMode & 1)
            bus -> writeByte(statReg, bus -> readByte(statReg) | 1);
        else
            bus -> writeByte(statReg, bus -> readByte(statReg) & (~1));

        if((newMode >> 1) & 1)
            bus -> writeByte(statReg, bus -> readByte(statReg) | (1 << 1));
        else
            bus -> writeByte(statReg, bus -> readByte(statReg) & (~(1 << 1)));           
    }

    bool GPU::isOn()
    {
        return (bus -> readByte(LCDControl) >> 7) & 1;
    }

    uint8_t GPU::getMode()
    {
        return bus -> readByte(statReg) & 0b11;
    }

    void GPU::update()
    {
        // If not on, then don't do anything
        if(!isOn())
            return;

        // Update the clock
        clock++;

        // Do stuff depending on the mode
        switch (getMode())
        {
        // HBlank mode
        case 0:
            // HBlank over
            if(clock >= 204)
            {
                clock -= 204;
                changeScanline();

                // We are going in the vertical blank mode
                if(bus -> readByte(lineReg) == 143)
                {
                    setMode(1);
                    // TODO: the original says we should request an interrupt here
                    cnt++;
                    drawFlag = true;
                }
                else
                    setMode(2);
            }
            break;
        // VBlank mode
        case 1:
            if(clock >= 456)
            {
                clock -= 456;
                if(bus -> readByte(lineReg) == 0)
                    setMode(2);
                else
                {
                    changeScanline();
                    if(bus -> readByte(lineReg) == 0)
                        setMode(2);
                }
            }
            break;
        
        // For accessing VRAM
        case 2:
            if(clock >= 172)
            {
                clock -= 172;
                setMode(0);
                renderTiles();
            }
            break;
        default:
            printf("\033[1;31m{E}: Unsuported GPU display mode %d.\n\033[0m", getMode());
            break;
        }
    }

    void GPU::renderTiles()
    {
        // We are not allowed to render outside the screen
        if(bus -> readByte(lineReg) >= 144)
            return;

        // Tile location depending on LCD register
        uint16_t where;

        // If location is signed
        bool is_signed;

        // Scrolling values and window values
        uint8_t scrollx = bus -> readByte(0xFF43), scrolly = bus -> readByte(0xFF42);
        uint8_t windowx = bus -> readByte(0xFF4B) - 7, windowy = bus -> readByte(0xFF4A);

        bool usingWindow = 0;

        // Get control register
        uint8_t control = bus -> readByte(LCDControl);

        // If we are using windows
        if((control >> 5) & 1)
        {
            if(bus -> readByte(lineReg) >= windowy)
                usingWindow = 1;
        }

        // Test bit for adressing locations
        if((control >> 4) & 1)
        {
            where = 0x8000;
            is_signed = false;
        }
        else
        {
            where = 0x8800;
            is_signed = true;
        }

        // Get tilemap location
        uint16_t tilemap;
        if(!usingWindow)
            tilemap = ((control >> 3) & 1) ? 0x9C00 : 0x9800;
        else
            tilemap = ((control >> 6) & 1) ? 0x9C00 : 0x9800;

        // Get background y location
        uint8_t y = scrolly + bus -> readByte(lineReg);
        if(usingWindow)
            y -= scrolly + windowy;

        // Row in the 32x32 sprite matrix
        uint16_t tileRow = (uint8_t)(y / 8);

        for(int i = 0; i < 160; i++)
        {
            uint8_t x = i + scrollx;
            if(usingWindow)
            {
                if(i >= windowx)
                    x -= scrollx + windowx;
            }

            // Get column with the X tile
            uint16_t tileColumn = x / 8;

            // Get the adress
            uint16_t addr = tilemap + tileRow * 32 + tileColumn;
            
            // Get the tile number from that
            int16_t tilenum;
            if(is_signed)
                tilenum = (int8_t) bus -> readByte(addr);
            else
                tilenum = bus -> readByte(addr);

            // Get the tile adress (16 bytes per tile)
            uint16_t tileAdress;
            if(!is_signed)
                tileAdress = where + tilenum * 16;
            else
                tileAdress = where + (tilenum + 128) * 16;
                

            // LNO column
            uint8_t lno = y % 8;
            uint8_t byte1 = bus -> readWord(tileAdress + lno * 2);
            uint8_t byte2 = bus -> readWord(tileAdress + lno * 2 + 1);

            // Byte 1 and 2 contain the data for the line, and bit 7 contains for x = 0
            uint8_t req_bit = 7 - (x % 8);
            uint8_t bit1 = (byte1 >> req_bit) & 1;
            uint8_t bit2 = (byte2 >> req_bit) & 1;
            uint8_t colorid = (bit1 << 1) | bit2;

            // Save the color
            pixels[i][bus -> readByte(lineReg)] = getColor(colorid, 0xFF47);
        }
    }

    int32_t GPU::getColor(int32_t id, uint16_t palette)
    {
        uint8_t data = bus -> readByte(palette);
        int hi = 2 * id + 1, lo = 2 * id;
        int bit1 = (data >> hi) & 1;
        int bit0 = (data >> lo) & 1;
        return (bit1 << 1) | bit0;
    }

    void GPU::changeScanline()
    {
        // If we can't go further, reset
        if(bus -> readByte(lineReg) == 153)
            bus -> writeByte(lineReg, 0);
        // Otherwise increment the line register
        else
            bus -> writeByte(lineReg, bus -> readByte(lineReg) + 1);
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

        memset(pixels, 0, 160 * 144);
    }
}
