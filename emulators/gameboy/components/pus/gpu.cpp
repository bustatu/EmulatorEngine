#include "gpu.h"

Gameboy_GPU::Gameboy_GPU()
{
    // Initialise VRAM
    vram = new uint8_t[0x2000];
}

uint8_t Gameboy_GPU::readByte(uint16_t addr)
{
    return vram[addr];
}

void Gameboy_GPU::draw(SDL_Texture* output, SDL_Renderer* tool)
{
    SDL_SetRenderTarget(tool, output);

    for(int i = 0; i < 8; i++)
    {
        for(int j = 0; j < 8; j++)
        {
            for(int k = 0; k < 8; k++)
            {
                SDL_SetRenderDrawColor(tool, 0x55 * tile[i][j][k], 0x55 * tile[i][j][k], 0x55 * tile[i][j][k], 0xFF);
                SDL_RenderDrawPoint(tool, j * 8, i * 8 + k);
            }
        }
    }

    SDL_SetRenderTarget(tool, NULL);
}

void Gameboy_GPU::writeByte(uint16_t addr, uint8_t val)
{
    // Normal write
    vram[addr] = val;

    // Not writing to the tile set storage
    if(addr >= 1800)   return;

    // Even adress
    uint16_t normal_addr = addr & 0xFFFE;

    // Tile row
    uint8_t byte1 = vram[normal_addr];
    uint8_t byte2 = vram[normal_addr + 1];

    // The tiles
    uint16_t tile_index = addr / 16;
    uint16_t row_index = (addr % 16) / 16;

    for(int index = 0; index < 8; index++)
    {
        uint8_t lsb = byte1 & (1 << (7 - index));
        uint8_t msb = byte2 & (1 << (7 - index));

        tile[tile_index][row_index][index] = 2 * (msb != 0) + (lsb != 0);
    }
}