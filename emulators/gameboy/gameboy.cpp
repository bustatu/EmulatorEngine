#include "gameboy.hpp"

namespace Gameboy
{
    void Emu::init()
    {
        Window* window = stateM -> getWindow();
    
        output = SDL_CreateTexture(stateM -> getWindow() -> getRenderer(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 160, 144);

        // Attach the components to their place
        bus.bios = &bios;
        bus.ram = &ram;
        bus.rom = &rom;
        bus.joypad = &joypad;
        bus.timer = &timer;
        bus.gpu = &gpu;
        cpu.bus = &bus;
        gpu.bus = &bus;

        // Reset execution timer
        executionTimer = 0;

        // Update the screen output (correct the aspect ratio)
        SDL_RenderSetLogicalSize(window -> getRenderer(), 160, 144);

        // Apply default config
        applyDefaultConfig();
    }

    void Emu::createDefaultConfig()
    {
        // Open output file
        std::ofstream fout("data/gameboy/general.json");

        // The JSON data
        nlohmann::json j_file;

        // Set bios as not found
        j_file["bios"]["disabled"] = true;
        j_file["bios"]["path"] = "UNDEFINED";

        // Write the JSON to the file
        fout << j_file;

        // Close the file
        fout.close();
    }

    void Emu::applyDefaultConfig()
    {
        // Read configs
        std::ifstream fin("data/gameboy/general.json");
        if(!fin.is_open())
        {
            printf("\033[1;36m{W}: Default general Gameboy config not found! Creating a new one...\n\033[0m");
            createDefaultConfig();
            fin.close();
            fin = std::ifstream("data/gameboy/general.json");
        }

        nlohmann::json j_file;

        fin >> j_file;
        
        // Load the BIOS from the config file
        if(!j_file["bios"]["disabled"])
        {
            bios.load(j_file["bios"]["path"]);
            if(!bios.isLoaded())
                printf("\033[1;36m{W}: BIOS loading failed! Booting with no BIOS...\n\033[0m"), cpu.skip_bios();
        }
        else cpu.skip_bios();
    }

    void Emu::loadROM(std::string path)
    {
        rom.load(path);
    }

    void Emu::resume()
    {
        printf("{I}: Gameboy emulator has resumed!\n");
        is_running = true;    
    }

    void Emu::pause()
    {
        printf("{I}: Gameboy emulator has been stopped!\n");
        is_running = false;
    }

    void Emu::update(double dt)
    {
        // Get window handler
        Window* window = stateM -> getWindow();

        // Do not execute more than 0.25s of delay (4 fps)
        dt = ((dt < 0.25) ? dt : 0.25);

        executionTimer += dt;

        // Check inputs and do interrupt if required
        for(uint8_t i = 0; i < 8; i++)
            joypad.updateButton(i, !window -> getKey(keys[i]));
        if(joypad.needsInterrupt())
            cpu.requestInterrupt(4);

        while(executionTimer >= 1.0 / freq)
        {
            // Update the components
            timer.update();
            if(timer.needsInterrupt())
                cpu.requestInterrupt(2);

            cpu.execute();
            gpu.update();

            executionTimer -= 1.0 / freq;
        }
    }

    void Emu::draw()
    {
        Window* window = stateM -> getWindow();

        // Update the output if needed
        gpu.draw(output, window -> getRenderer());

        // Update the screen output
        SDL_RenderCopy(window -> getRenderer(), output, NULL, NULL);
    }
}