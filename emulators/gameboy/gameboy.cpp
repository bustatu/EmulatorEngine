#include "gameboy.h"

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
        cpu -> update(dt);
    }

    void Emu::draw()
    {
        Window* window = stateM -> getWindow();
        SDL_RenderCopy(window -> getRenderer(), output, NULL, NULL);
    }
}