#include "audio.h"

Audio::Audio()
{
    if(SDL_Init(SDL_INIT_AUDIO) < 0)
        printf("{E}: Audio init failed! %s", SDL_GetError());

    SDL_zero(wanted);
    wanted.freq = 15360;
    wanted.samples = 256;
    wanted.format = AUDIO_S8;
    wanted.channels = 1;

    SDL_OpenAudio(&wanted, NULL);

    SDL_PauseAudio(0);
}

void Audio::push(int8_t* where, int32_t size)
{
    SDL_QueueAudio(1, where, size);
}