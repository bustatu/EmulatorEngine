#ifndef AUDIO_H
#define AUDIO_H

#include <SDL2/SDL.h>
#include <iostream>

class Audio
{
private:
    // Wanted audio type
    SDL_AudioSpec wanted;

    // If audio is paused
    bool paused = true;
    
public:
    // Constructor
    Audio();

    // Pause audio
    void pauseAudio();

    // Resume audio
    void resumeAudio();

    // Prepare audio
    void prepare(SDL_AudioSpec spec);

    // Get current audiospec
    SDL_AudioSpec getSpec();

    // Check if audio is paused
    bool isPaused();

    // Reset the audio
    void reset();

    // Destructor
    ~Audio();
    
    // Push sound to the audio buffer
    void push(int8_t *from, int32_t size);
};

#endif