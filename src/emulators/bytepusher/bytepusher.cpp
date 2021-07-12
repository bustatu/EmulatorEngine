#include "bytepusher.h"

// Sets surface pixel
void set_pixel(SDL_Surface *surface, int x, int y, uint32_t pixel)
{
    uint32_t *const target_pixel = (uint32_t*)((uint8_t*) surface -> pixels + y * surface -> pitch + x * surface->format->BytesPerPixel);
    *target_pixel = pixel;
}   

// Get value from the memory
uint32_t BytePusherEmu::getVal(uint32_t pc, uint32_t length)
{
    uint32_t v = 0;
    while(length--)
        v = (v << 8) + memory[pc++];
    return v;
}


BytePusherEmu::BytePusherEmu()
{
    // Create RAM
    memory = new uint8_t[0xFFFFFF];

    // Create palette
    for(int i = 0; i <= 215; i++)
    {
        // Formula for RGB format
        int b = i % 6;
        int g = ((i - b) / 6) % 6;
        int r = ((i - b - 6 * g) / 36) % 6;
        pallete[i] = 0xFF000000 | ((r * 0x33) << 16) | ((g * 0x33) << 8) | (b * 0x33);
    }
    // These pixels are black
    for(int i = 216; i <= 255; i++)
        pallete[i] = 0xFF000000;

    // Init sound
    sound = new Audio();
}

void BytePusherEmu::load(std::string path)
{
    // Create reader
    std::ifstream fin(path.c_str(), std::ios::in | std::ios::binary);

    // Clear memory
    memset(memory, 0, sizeof(memory[0]) * 0xFFFFFF);

    // If file exists
    if(fin.good())
    {
        // Starting ROM load
        printf("{I}: Loading BytePusher ROM at: %s!\n", path.c_str());

        // Get file size
        fin.seekg(0, std::ios::end);
        int file_size = fin.tellg();

        // Seek to start
        fin.seekg(0, std::ios::beg);

        // Read file if valid
        if (file_size <= 0x100000)
        {
            // Read the entire file contents to the memory
            fin.read(reinterpret_cast<char*>(memory), file_size);

            // Message so we know everything's OK.
            printf("{I}: Read file with size: %d bytes.\n", file_size);
        }
        else {
            // File too big
            printf("{E}: File too big!\n");
            isQuit = true;        
        }
    }
    else {
        // File not Found
        printf("{E}: File does not exist!\n");
        isQuit = true;
    }
}

bool BytePusherEmu::isRunning()
{
    return !isQuit;
}

void BytePusherEmu::quit()
{
    isQuit = true;
}

void BytePusherEmu::updateKey(SDL_Keycode key, int value)
{
    // Go trough all the keys and set / unset the good one.
    for(int i = 0; i < 0x10; i++)
        if(keys[i] == key)
            // Unset key
            if(value == 0) memory[1 - i / 8] &= ~(1 << (i % 8));
            // Set key
            else memory[1 - i / 8] |= (1 << (i % 8)); 
}

void BytePusherEmu::innerLoop()
{
    // Get execution location from memory
    uint32_t pc = getVal(2, 3);

    // Steps per cycle
    for(int i = 0; i < 0x10000 && !isQuit; i++)
    {
        // memory[B] = memory[A], pc = memory[C]
        memory[getVal(pc + 3, 3)] = memory[getVal(pc, 3)];
        pc = getVal(pc + 6, 3);
    }
}

void BytePusherEmu::update(float dt)
{
    if(!isQuit)
    {
        // Update delta time
        counter += dt;

        // 60 Hz, forced
        while(counter >= 1.0 / 60 && !isQuit)
        {
            // Do the inner loop
            innerLoop();

            // Check memory bank related to the display
            uint8_t* frame = memory + (getVal(5, 1) << 16);
            for(int i = 0; i < 256; i++)
            {
                for(int j = 0; j < 256; j++)
                    // Set pixel from palette
                    set_pixel(result, i, j, pallete[frame[(j << 8) | i]]);
            }

            // Push audio section to the sound engine
            sound -> push((int8_t*)(memory + (getVal(6, 2) << 8)), 256);

            // 60 Hz, forced
            counter -= 1.0 / 60;
        }

        // Go trough all the keys and set / unset the good one.
        for(int i = 0; i < 0x10; i++)
        {
            // If key is down mark it as such
            if(Window::get() -> isKeyDown[keys[i]]) memory[1 - i / 8] |= (1 << (i % 8));
            else memory[1 - i / 8] &= ~(1 << (i % 8));
        }
    }
}

void BytePusherEmu::draw(Window* win)
{
    if(!isQuit)
    {
        // Copy the surface to the screen
        win -> getRenderer() -> render(result);
    }
}