#include "gameboy.h"

void GAMEBOY_EMU::init()
{
    // Initialise components
    if(bios == nullptr)
        bios = new Gameboy_BIOS();
    if(bus == nullptr)
        bus = new Gameboy_Bus();
    if(cpu == nullptr)
        cpu = new Gameboy_CPU();
    if(ram == nullptr)
        ram = new Gameboy_RAM();
    if(rom == nullptr)
        rom = new Gameboy_ROM();
    if(gpu == nullptr)
        gpu = new Gameboy_GPU();
    output = SDL_CreateTexture(stateM -> getWindow() -> getRenderer(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 160, 144);

    // Attach the components to their place
    bus -> attachBIOS(bios);
    bus -> attachRAM(ram);
    bus -> attachROM(rom);
    bus -> attachGPU(gpu);
    cpu -> attachBus(bus);
}

void GAMEBOY_EMU::loadBIOS(std::string path)
{
    if(bios == nullptr)
        bios = new Gameboy_BIOS();
    bios -> load(path);
}

void GAMEBOY_EMU::resume()
{
    printf("{I}: Gameboy emulator has resumed!\n");
    is_running = true;    
}

void GAMEBOY_EMU::pause()
{
    printf("{I}: Gameboy emulator has been stopped!\n");
    is_running = false;
}

void GAMEBOY_EMU::update(double dt)
{
    cpu -> update(dt);
}

void GAMEBOY_EMU::draw()
{
    Window* window = stateM -> getWindow();
    gpu -> draw(output, window -> getRenderer());
    SDL_RenderCopy(window -> getRenderer(), output, NULL, NULL);
}