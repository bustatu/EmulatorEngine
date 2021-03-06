#include "chip8.hpp"

#ifndef M_PI
    // PI is not standardly defined everywhere
    #define M_PI 3.14159265358979323846
#endif

namespace CHIP8
{
    void Emu::update(double dt)
    {
        Window* window = stateM -> getWindow();

        // If the emulator didn't stop
        if(is_running)
        {
            if(!use_dynarec)
            {
                // Update the VM
                if(vm -> get_state() == 1)
                {
                    vm -> updateKeys(window);
                    vm -> update(dt);

                    if(vm -> get_timer(1) > 0)
                        window -> getAudio() -> resumeAudio();
                    else
                        window -> getAudio() -> pauseAudio();
                }
                else
                {
                    // Execution ended
                    is_running = false;

                    // Do diagnostics
                    printf("\033[1;31m");
                    switch(vm -> get_state())
                    {
                        case 2:
                            printf("{E}: Execution ended because PC got out of bounds.\n");
                            break;
                        case 3:
                            printf("{E}: Execution ended because the VM encountered an unknown opcode.\n");
                            break;
                    }
                    printf("\033[0m");

                    printf("{I}: Error code: %d\n", vm -> get_state());
                }
            }
        }

        // Toggle running state if escape has been pressed
        if(window -> getKeyPressed(SDLK_ESCAPE))
            is_running = !is_running;
    }

    void Emu::applyDefaultConfig()
    {
        // Read configs
        std::ifstream fin("data/chip8/general.json");
        if(!fin.is_open())
        {
            fin.close();
            printf("\033[1;36m{W}: Default general CHIP8 config not found! Creating a new one...\n\033[0m");
            createDefaultConfig();
            fin = std::ifstream("data/chip8/general.json");
        }

        nlohmann::json j_file;

        fin >> j_file;

        if(j_file["color"]["r"] != nullptr && j_file["color"]["g"] != nullptr && j_file["color"]["b"] != nullptr)
            graphics.setForeground({j_file["color"]["r"], j_file["color"]["g"], j_file["color"]["b"], 0xFF});

        if(j_file["background"]["r"] != nullptr && j_file["background"]["g"] != nullptr && j_file["background"]["b"] != nullptr)
            graphics.setBackground({j_file["background"]["r"], j_file["background"]["g"], j_file["background"]["b"], 0xFF});
        
        fin.close();
    }

    void Emu::createDefaultConfig()
    {
        // Open output file
        std::ofstream fout("data/chip8/general.json");

        // The JSON data
        nlohmann::json j_file;

        // Set default background and foreground colors
        j_file["color"]["r"] = j_file["color"]["g"] = j_file["color"]["b"] = 0x88;
        j_file["background"]["r"] = j_file["background"]["g"] = j_file["background"]["b"] = 0x10;

        // Write the JSON to the file
        fout << j_file;

        // Close the file
        fout.close();
    }

    void Emu::init()
    {
        Window* window = stateM -> getWindow();
        Audio* audio = window -> getAudio();

        // Create audio specs for this emu
        SDL_AudioSpec wanted;
        SDL_zero(wanted);
        wanted.freq = 44100;
        wanted.samples = 256;
        wanted.format = AUDIO_S8;
        wanted.channels = 1;
        
        // Sine wave callback function
        wanted.callback = [](void* userdata, uint8_t* stream, int length)
        {
            for(int i = 0; i < length; i++)
                stream[i] = (127 * std::sin(i * M_PI * 2 * 450.0 / 44100));
        };

        // Prepare audio for usage
        audio -> prepare(wanted);

        // Pause the sound, only gets resumed when the sound counter is positive
        audio -> pauseAudio();

        // Update the screen output (correct the aspect ratio)
        SDL_RenderSetLogicalSize(window -> getRenderer(), 128, 64);
    }

    void Emu::resume()
    {
        printf("{I}: CHIP8 emulator has resumed!\n");
        is_running = true;
    }

    void Emu::pause()
    {
        printf("{I}: CHIP8 emulator has been stopped!\n");
        is_running = false;
    }

    void Emu::draw()
    {
        Window* window = stateM -> getWindow();

        // Do the drawing internally
        if(!use_dynarec)
            vm -> draw(output, window -> getRenderer());

        // Update only if the output exists
        if(output != nullptr)
            // Update the screen output
            SDL_RenderCopy(window -> getRenderer(), output, NULL, NULL);
    }

    void Emu::load(std::string path)
    {
        // Create new VM
        if(!use_dynarec)
        {
            if(vm != nullptr)
                delete vm;
            
            exit(0);
            // TODO: THIS BREAKS
            vm = new VM();
       
            // Start VM
            vm -> boot();
        }

        // Apply default config
        applyDefaultConfig();

        // Open file for reading
        std::ifstream file(path.c_str(), std::ios::in | std::ios::binary | std::ios::ate);

        if(!file.is_open())
        {
            printf("\033[1;31m{E}: File at path %s does not exist!\n\033[0m", path.c_str());
            return;
        }

        // Write info
        printf("{I}: ROM %s is being loaded...\n", path.c_str());

        // Get file size
        file.seekg(0, std::ios::end);
        int32_t fileSize = file.tellg();
        file.seekg(0, std::ios::beg);

        if(fileSize == -1)
            printf("\033[1;31m{E}: File at path %s does not exist!\n\033[0m", path.c_str());
        else
        {
            // Read the entirety of the file contents into a buffer
            uint8_t* data = new uint8_t[fileSize];
            file.read(reinterpret_cast<char*>(data), fileSize);

            // Attempt to write the data into the VM
            // Most files start at 0x200 in memory
            // TODO: Implement non-standard offsets
            if(!use_dynarec)
            {
                if(vm -> writeBuffer(vm -> get_rom_start(), data, fileSize) != 0)
                {
                    printf("\033[1;31m{E}: An error occured while loading the ROM!\n\033[0m");
                    is_running = false;
                }
                else
                {
                    printf("{I}: ROM with size %d loaded successfully!\n", fileSize);
                    is_running = true;
                }
            }
            else
            {
                printf("{I}: Dynarec ROM loading here...\n");
            }

            // Deallocate data from memory
            delete[] data;
        }

        // Close the file
        file.close();
    }

    void Emu::destroy()
    {
        printf("{I}: Destroying CHIP8 state!\n");
    }
};