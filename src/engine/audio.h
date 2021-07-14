#ifndef ENGINE_AUDIO_H
#define ENGINE_AUDIO_H

#include <SDL2/SDL.h>
#include <iostream>

class Audio
{
private:
    SDL_AudioSpec wanted;
public:
    Audio();
    void push(int8_t *from, int32_t size);
};

#endif