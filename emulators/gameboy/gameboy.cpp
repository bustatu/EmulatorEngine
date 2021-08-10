#include "gameboy.h"

void GAMEBOY_EMU::init()
{
    // Initialise components
    if(bios == nullptr)
        bios = new Gameboy_BIOS();
    if(cpu == nullptr)
        cpu = new Gameboy_CPU();
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

}

void GAMEBOY_EMU::draw()
{

}