
#include "renderer.h"

void SoftwareRenderer::drawStart()
{
    SDL_FillRect(SDL_GetWindowSurface(parentWin), NULL, 0x000000);
}

void SoftwareRenderer::render(SDL_Surface* surf)
{
    SDL_BlitSurface(surf, NULL, SDL_GetWindowSurface(parentWin), NULL);
}

void SoftwareRenderer::drawEnd()
{
    SDL_UpdateWindowSurface(parentWin);
}

void AcceleratedRenderer::render(SDL_Surface* surf)
{
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surf);

    SDL_RenderCopy(renderer, texture, NULL, NULL);

    SDL_DestroyTexture(texture);
}

void AcceleratedRenderer::drawStart()
{
    SDL_RenderClear(renderer);
}

void AcceleratedRenderer::drawEnd()
{
    SDL_RenderPresent(renderer);
}