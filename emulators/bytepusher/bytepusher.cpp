#include "bytepusher.h"

namespace Bytepusher
{
    void Emu::update(double dt)
    {
        Window* window = stateM -> getWindow();

        if(is_running)
        {
            // If running
            if(vm -> getState() == 1)
                vm -> update(dt, window);
            else
                pause();
        }

        // Toggle running state if escape has been pressed
        if(window -> getKeyPressed(SDLK_ESCAPE))
            is_running = !is_running;
    }

    void Emu::draw()
    {
        Window* window = stateM -> getWindow();

        // If output doesnt exist create it
        if(output == nullptr)
            output = SDL_CreateTexture(window -> getRenderer(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 256, 256);

        // Update the VM output
        vm -> draw(output, window -> getRenderer());
        
        // Update the screen output (correct the aspect ratio)
        SDL_Rect* rect = new SDL_Rect();
        rect -> w = rect -> h = window -> getSize().second;
        rect -> y = 0;
        rect -> x = (window -> getSize().first - rect -> w) / 2;

        if(rect -> w > window -> getSize().first)
        {
            rect -> w = rect -> h = window -> getSize().first;
            rect -> x = 0;
            rect -> y = (window -> getSize().second - rect -> h) / 2;
        }

        // Update the screen output
        SDL_RenderCopy(window -> getRenderer(), output, NULL, rect);

        // Delete the allocated rect
        delete rect;
    }

    void Emu::init()
    {
        Window* window = stateM -> getWindow();

        // Create audio specs for this emu
        SDL_AudioSpec wanted;
        SDL_zero(wanted);
        wanted.freq = 15360;
        wanted.samples = 256;
        wanted.format = AUDIO_S8;
        wanted.channels = 1;

        // Prepare audio for usage
        window -> getAudio() -> prepare(wanted);

        // Unpause the sound
        window -> getAudio() -> resumeAudio();    
    }

    void Emu::resume()
    {
        printf("{I}: Bytepusher emulator has resumed!\n");
        is_running = true;    
    }

    void Emu::pause()
    {
        printf("{I}: Bytepusher emulator has been stopped!\n");
        is_running = false;    
    }

    void Emu::load(std::string path)
    {
        // Create new VM
        if(vm != nullptr)
            delete vm;
        vm = new VM();

        // Start VM
        vm -> boot();

        // Open file for reading
        std::ifstream file(path.c_str(), std::ios::in | std::ios::binary | std::ios::ate);

        // Write info
        printf("{I}: ROM %s is being loaded...\n", path.c_str());

        // Get file size
        file.seekg(0, std::ios::end);
        uint32_t fileSize = file.tellg();
        file.seekg(0, std::ios::beg);

        if(fileSize == -1)
        {
            printf("\033[1;31m");
            printf("{E}: File at path %s does not exist!\n", path.c_str());
            printf("\033[0m");       
        }
        else
        {
            // Read the entirety of the file contents into a buffer
            uint8_t* data = new uint8_t[fileSize];
            file.read(reinterpret_cast<char*>(data), fileSize);

            // Attempt to write the data into the VM
            // TODO: Implement memory bank loading
            if(vm -> writeBuffer(0, data, fileSize) != 0)
            {
                printf("\033[1;31m");
                printf("{E}: An error occured while loading the ROM!\n");
                printf("\033[0m");
                is_running = false;
            }
            else
            {
                printf("{I}: ROM with size %d loaded successfully!\n", fileSize);
                is_running = true;
            }

            // Deallocate data from memory
            delete[] data;
        }
    }
}