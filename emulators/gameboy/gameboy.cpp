#include "gameboy.hpp"

namespace Gameboy
{
    void Emu::init()
    {
        // Initialise components
        if(bios == nullptr)
            bios = new BIOS();
        if(bus == nullptr)
            bus = new Bus();
        if(cpu == nullptr)
            cpu = new CPU();
        if(ram == nullptr)
            ram = new RAM();
        if(rom == nullptr)
            rom = new ROM();
        if(gpu == nullptr)
            gpu = new GPU();
        output = SDL_CreateTexture(stateM -> getWindow() -> getRenderer(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 160, 144);

        // Attach the components to their place
        bus -> attachBIOS(bios);
        bus -> attachRAM(ram);
        bus -> attachROM(rom);
        gpu -> attachBus(bus);
        cpu -> attachBus(bus);

        // Reset execution timer
        executionTimer = 0;
    }

    void Emu::loadBIOS(std::string path)
    {
        if(bios == nullptr)
            bios = new BIOS();
        bios -> load(path);
    }

    void Emu::loadROM(std::string path)
    {
        if(rom == nullptr)
            rom = new ROM();
        rom -> load(path);
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
        executionTimer += dt;

        while(executionTimer >= 1.0 / freq)
        {
            // Update the CPU and the GPU
            cpu -> execute();
            gpu -> update();

            executionTimer -= 1.0 / freq;
        }
    }

    void Emu::draw()
    {
        Window* window = stateM -> getWindow();

        // Update the output if needed
        gpu -> draw(output, window -> getRenderer());

        // Update the screen output (correct the aspect ratio)
        SDL_Rect* rect = new SDL_Rect();
        rect -> h = window -> getSize().second;
        rect -> w = rect -> h * 160 / 144;

        rect -> x = (window -> getSize().first - rect -> w) / 2;
        rect -> y = 0;

        // Update the screen output
        SDL_RenderCopy(window -> getRenderer(), output, NULL, rect);

        // Delete the allocated rect
        delete rect;
    }
}