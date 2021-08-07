#include "chip8.h"

void CHIP8_EMU::update(double dt)
{
    Window* window = stateM -> getWindow();

    // If the emulator didn't stop
    if(is_running)
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

    // Toggle running state if escape has been pressed
    if(window -> getKeyPressed(SDLK_ESCAPE))
        is_running = !is_running;
}

void CHIP8_EMU::init()
{
    Window* window = stateM -> getWindow();

    // Resize the window to match the emu
    // TODO: find a better way to do this
    window -> resize(640, 320);

    // Create audio specs for this emu
    SDL_AudioSpec wanted;
    SDL_zero(wanted);
    wanted.freq = 45500;
    wanted.samples = 256;
    wanted.format = AUDIO_S8;
    wanted.channels = 1;
    // Sine wave callback function
    wanted.callback = [](void* userdata, uint8_t* stream, int length)
    {
        for (int i = 0; i < length; i++)
            stream[i] = (uint8_t)(127 * sin(2.0 * M_PI * i * 604.1 / 45500));
    };

    // Prepare audio for usage
    window -> getAudio() -> prepare(wanted);

    // Pause the sound, only gets resumed when the sound counter is positive
    window -> getAudio() -> pauseAudio();
}


void CHIP8_EMU::resume()
{
    printf("{I}: CHIP8 emulator has resumed!\n");
    is_running = true;
}

void CHIP8_EMU::pause()
{
    printf("{I}: CHIP8 emulator has been stopped!\n");
    is_running = false;
}

void CHIP8_EMU::draw()
{
    Window* window = stateM -> getWindow();

    if(output == nullptr)
        output = SDL_CreateTexture(window -> getRenderer(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, vm -> getSize().first, vm -> getSize().second);

    // Do the drawing internally
    vm -> draw(output, window -> getRenderer());

    // Update the screen output (correct the aspect ratio)
    SDL_Rect* rect = new SDL_Rect();
    rect -> h = window -> getSize().second;
    rect -> w = rect -> h * 2;

    if(window -> getSize().first >= rect -> w)
    {
        rect -> x = (window -> getSize().first - rect -> w) / 2;
        rect -> y = 0;
    }
    else {
        rect -> w = window -> getSize().first;
        rect -> h = rect -> w / 2;
        rect -> y = (window -> getSize().second - rect -> h) / 2;
        rect -> x = 0;
    }

    // Update the screen output
    SDL_RenderCopy(window -> getRenderer(), output, NULL, rect);

    // Delete the allocated rect
    delete rect;
}

void CHIP8_EMU::load(std::string path)
{
    // Create new VM
    if(vm != nullptr)
        delete vm;
    vm = new CHIP8_VM();

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
        printf("\033[1;31m{E}: File at path %s does not exist!\n\033[0m", path.c_str());
    else
    {
        // Read the entirety of the file contents into a buffer
        uint8_t* data = new uint8_t[fileSize];
        file.read(reinterpret_cast<char*>(data), fileSize);

        // Attempt to write the data into the VM
        // Most files start at 0x200 in memory
        // TODO: Implement non-standard offsets
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

        // Deallocate data from memory
        delete[] data;
    }
}