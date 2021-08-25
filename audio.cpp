#include "audio.hpp"

Audio::Audio()
{
    if(SDL_Init(SDL_INIT_AUDIO) < 0)
        printf("{E}: Audio init failed! %s", SDL_GetError());

    SDL_zero(wanted);
}

void Audio::pauseAudio()
{
    if(paused == false)
        SDL_PauseAudio(1);
    paused = true;
}

SDL_AudioSpec Audio::getSpec()
{
    return wanted;
}

void Audio::prepare(SDL_AudioSpec spec)
{
    wanted = spec;

    SDL_CloseAudio();
    SDL_OpenAudio(&wanted, NULL);
}

void Audio::resumeAudio()
{
    if(paused == true)
        SDL_PauseAudio(0);
    paused = false;
}

bool Audio::isPaused()
{
    return paused;
}

Audio::~Audio()
{
    SDL_CloseAudio();
}

void Audio::push(int8_t* where, int32_t size)
{
    SDL_LockAudio();
    SDL_QueueAudio(1, where, size);
    SDL_UnlockAudio();
}