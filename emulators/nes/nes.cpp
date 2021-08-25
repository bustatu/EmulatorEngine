#include "nes.hpp"

namespace NES
{
    void Emu::update(double dt)
    {
        Window* window = stateM -> getWindow();

        // If the emulator didn't stop
        if(is_running)
        {

        }

        // Toggle running state if escape has been pressed
        if(window -> getKeyPressed(SDLK_ESCAPE))
            is_running = !is_running;
    }

    void Emu::init()
    {
        Window* window = stateM -> getWindow();
    }

    void Emu::resume()
    {
        printf("{I}: NES emulator has resumed!\n");
        is_running = true;
    }

    void Emu::pause()
    {
        printf("{I}: NES emulator has been stopped!\n");
        is_running = false;
    }

    void Emu::draw()
    {
        Window* window = stateM -> getWindow();

        // Update only if the output exists
        if(output != nullptr)
        {
            // Update the screen output
            SDL_RenderCopy(window -> getRenderer(), output, NULL, NULL);
        }
    }

    void Emu::load(std::string path)
    {
        // Open file for reading
        std::ifstream file(path.c_str(), std::ios::in | std::ios::binary | std::ios::ate);

        // Write info
        printf("{I}: ROM %s is being loaded...\n", path.c_str());

        // Get file size
        file.seekg(0, std::ios::end);
        uint32_t fileSize = file.tellg();
        file.seekg(0, std::ios::beg);

        if(fileSize == -1)
            printf("\033[1;31m{E}: File at path %s does not exist!\n\033[0m", path.c_str());
        else
        {
            // Read the entirety of the file contents into a buffer
            uint8_t* data = new uint8_t[fileSize];
            file.read(reinterpret_cast<char*>(data), fileSize);

            // Attempt to write the data into the VM
            if(false)
            {
                printf("\033[1;31m{E}: An error occured while loading the ROM!\n\033[0m");
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
};