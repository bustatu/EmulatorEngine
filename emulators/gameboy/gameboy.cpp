#include "gameboy.hpp"

namespace Gameboy
{
    void Emu::init()
    {
        Window* window = stateM -> getWindow();

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
        if(timer == nullptr)
            timer = new Timer();
        if(joypad == nullptr)
            joypad = new Joypad();       
        output = SDL_CreateTexture(stateM -> getWindow() -> getRenderer(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 160, 144);

        // Attach the components to their place
        bus -> bios = bios;
        bus -> ram = ram;
        bus -> rom = rom;
        bus -> joypad = joypad;
        bus -> timer = timer;
        gpu -> attachBus(bus);
        cpu -> attachBus(bus);

        // Reset execution timer
        executionTimer = 0;

        // Update the screen output (correct the aspect ratio)
        SDL_RenderSetLogicalSize(window -> getRenderer(), 160, 144);
    }

    void Emu::skip_bios()
    {
        cpu -> skip_bios();
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
        // Get window handler
        Window* window = stateM -> getWindow();

        // Do not execute more than 0.25s of delay (4 fps)
        dt = ((dt < 0.25) ? dt : 0.25);

        executionTimer += dt;

        // Check inputs and do interrupt if required
        for(uint8_t i = 0; i < 8; i++)
            joypad -> updateButton(i, window -> getKey(keys[i]));
        if(joypad -> needsInterrupt())
            cpu -> requestInterrupt(4);

        while(executionTimer >= 1.0 / freq)
        {
            // Update the components
            timer -> update();
            if(timer -> needsInterrupt())
                cpu -> requestInterrupt(2);

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

        // Update the screen output
        SDL_RenderCopy(window -> getRenderer(), output, NULL, NULL);
    }
}