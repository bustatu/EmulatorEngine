#include "vm.h"

#define N000(opcode) ((opcode & 0xF000) >> 12)
#define NNN(opcode) (opcode & 0xFFF)
#define N00(opcode) ((opcode & 0xF00) >> 8)
#define NN(opcode) (opcode & 0xFF)
#define N0(opcode) ((opcode & 0xF0) >> 4)
#define N(opcode) (opcode & 0xF)

CHIP8_VM::CHIP8_VM()
{
    // Allocate RAM, registers, keys and stack
    ram = new uint8_t[ram_size];
    V = new uint8_t[0x10];
    stack = new uint16_t[stack_size];
    key = new uint8_t[0x10];
    user_flags = new uint8_t[0x10];

    // Create output
    screen_w = 64;
    screen_h = 32;
    gfx = new uint8_t[64 * 32];

    // Reset timers
    deltaTimer = execTimer = 0;

    // Set default colors
    frg.r = 0x88, frg.g = 0x88, frg.b = 0x88, frg.a = 0xFF;
    bkg.r = 0x10, bkg.g = 0x10, bkg.b = 0x10, bkg.a = 0xFF;

    // Stopped
    state = 0;

    // Set random seed time
    srand(time(NULL));
}

std::pair<int, int> CHIP8_VM::getSize()
{
    return std::make_pair(screen_w, screen_h);
}

CHIP8_VM::~CHIP8_VM()
{
    // Free RAM
    delete ram;
}

uint16_t CHIP8_VM::get_rom_start()
{
    // Return rom_start
    return rom_start;
}

void CHIP8_VM::draw(SDL_Texture* target, SDL_Renderer* tool)
{
    // Do the drawing only if the screen actually needs to be updated
    if(draw_flag)
    {
        SDL_SetRenderTarget(tool, target);
        for(uint32_t x = 0; x < screen_w; x++)
        {
            for(uint32_t y = 0; y < screen_h; y++)
            {
                if(gfx[y * screen_w + x])
                    SDL_SetRenderDrawColor(tool, frg.r, frg.g, frg.b, frg.a);
                else
                    SDL_SetRenderDrawColor(tool, bkg.r, bkg.g, bkg.b, bkg.a);

                SDL_RenderDrawPoint(tool, x, y);
            }
        }
        SDL_SetRenderTarget(tool, NULL);
    }
}

uint8_t CHIP8_VM::get_state()
{
    // Return state
    return state;
}

void CHIP8_VM::set_rom_start(uint16_t offset)
{
    // Set ROM start to offset
    rom_start = offset;
}

void CHIP8_VM::set_reg(uint8_t who, uint8_t what)
{
    // Set register who to what
    V[who] = what;
}

void CHIP8_VM::set_I(uint16_t what)
{
    // Set register I to what
    I = what;
}

uint8_t CHIP8_VM::get_key(uint8_t who)
{
    return key[who];
}

void CHIP8_VM::set_key(uint8_t who, uint8_t what)
{
    key[who] = what;
}

void CHIP8_VM::boot()
{
    // Clear everything
    memset(gfx, 0, screen_h * screen_w);
    memset(stack, 0, stack_size);
    memset(ram, 0, ram_size);
    memset(key, 0, 0x10);
    memset(V, 0, 0x10);
    memset(user_flags, 0, 0x10);

    // Load font
    uint8_t font[] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0,   // 0
        0x20, 0x60, 0x20, 0x20, 0x70,   // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0,   // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0,   // 3
        0x90, 0x90, 0xF0, 0x10, 0x10,   // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0,   // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0,   // 6
        0xF0, 0x10, 0x20, 0x40, 0x40,   // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0,   // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0,   // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90,   // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0,   // B
        0xF0, 0x80, 0x80, 0x80, 0xF0,   // C
        0xE0, 0x90, 0x90, 0x90, 0xE0,   // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0,   // E
        0xF0, 0x80, 0xF0, 0x80, 0x80    // F
    };
    memcpy(ram, font, 5 * 16);

    // Set PC to the start of the ROM
    PC = rom_start;

    // Clear stack
    sp = 0;

    // Do not update the screen
    draw_flag = false;
    
    // Clear timers (0 - delay, 1 - sound)
    timers[0] = timers[1] = 0;

    // Running
    state = 1;
}

// Draws sprite at position (va, vb) that has the height of vc
void CHIP8_VM::DRW(uint8_t va, uint8_t vb, uint8_t vc)
{
    uint8_t x = V[N00(opcode)];
    uint8_t y = V[N0(opcode)];
    uint8_t n = N(opcode);
    uint8_t pixel;

    //Set the F register to 0
    V[0xF] = 0;

    // Set the draw flag
    draw_flag = true;

    //Go trough the sprite lines
    for(int32_t yline = 0; yline < n; yline++)
    {
        //Read pixel from memory
        pixel = ram[I + yline];

        //Go trough the sprite columns
        for(int32_t xline = 0; xline < 8; xline++)
        {
            //If the pixel needs to be set
            if((pixel & (0x80 >> xline)) != 0)
            {
                // Position in the array
                int32_t pos = (x + xline + ((y + yline) * screen_w)) % (screen_w * screen_h);

                //If the pixel was already set, set VF to 1
                if(gfx[pos] == 1)
                    V[0xF] = 1;

                //XOR the pixel
                gfx[pos] ^= 1;
            }
        }
    }
}

void CHIP8_VM::set_timer(uint8_t who, uint8_t what)
{
    timers[who] = what;
}

uint8_t CHIP8_VM::get_timer(uint8_t who)
{
    return timers[who];
}

void CHIP8_VM::execute(uint16_t opcode)
{
    switch(N000(opcode))
    {
        case 0x0:
            switch(NN(opcode))
            {
                case 0xE0:
                    memset(gfx, 0, screen_h * screen_w);
                    break;
                case 0xEE:
                    PC = stack[sp--];
                    break;
                default:
                    unknown(opcode);
                    break;
            }
            break;
        case 0x1:
            PC = NNN(opcode);
            break;
        case 0x2:
            stack[++sp] = PC;
            PC = NNN(opcode);
            break;
        case 0x3:
            if(V[N00(opcode)] == NN(opcode))
                PC += 2;
            break;
        case 0x4:
            if(V[N00(opcode)] != NN(opcode))
                PC += 2;
            break;
        case 0x5:
            if(V[N00(opcode)] == V[N0(opcode)])
                PC += 2;
            break;
        case 0x6:
            set_reg(N00(opcode), NN(opcode));
            break;
        case 0x7:
            V[N00(opcode)] += NN(opcode);
            break;
        case 0x8:
            switch(N(opcode))
            {
                case 0x0:
                    V[N00(opcode)] = V[N0(opcode)];
                    break;
                case 0x1:
                    V[N00(opcode)] |= V[N0(opcode)];
                    break;
                case 0x2:
                    V[N00(opcode)] &= V[N0(opcode)];
                    break;
                case 0x3:
                    V[N00(opcode)] ^= V[N0(opcode)];
                    break;
                case 0x4:
                    // Overflow flag
                    V[0xF] = ((0xFF - V[N0(opcode)]) < V[N00(opcode)]);

                    // Add
                    V[N00(opcode)] += V[N0(opcode)];
                    break;
                case 0x5:
                    // Borrow flag
                    V[0xF] = (V[N00(opcode)] >= V[N0(opcode)]);

                    // Subtract
                    V[N00(opcode)] -= V[N0(opcode)];
                    break;
                case 0x6:
                    // Least significant bit
                    V[0xF] = V[N00(opcode)] & 1;

                    // Shift right
                    V[N00(opcode)] >>= 1;
                    break;
                case 0x7:
                    // Borrow flag
                    V[0xF] = (V[N00(opcode)] <= V[N0(opcode)]);

                    // Subtract
                    V[N00(opcode)] = V[N0(opcode)] - V[N00(opcode)];
                    break;
                case 0xE:
                    // Most significant bit
                    V[0xF] = V[N00(opcode)] >> 7;

                    // Shift left
                    V[N00(opcode)] <<= 1;
                    break;
                default:
                    unknown(opcode);
                    break;
            }
            break;
        case 0x9:
            if(V[N00(opcode)] != V[N0(opcode)])
                PC += 2;
            break;
        case 0xA:
            set_I(NNN(opcode));
            break;
        case 0xB:
            PC = V[0] + NNN(opcode);
            break;
        case 0xC:
            V[N00(opcode)] = rand() & NN(opcode);
            break;
        case 0xD:
            DRW(N00(opcode), N0(opcode), N(opcode));
            break;
        case 0xE:
            switch(NN(opcode))
            {
                case 0x9E:
                    if(get_key(V[N00(opcode)]) == true)
                        PC += 2;
                    break;
                case 0xA1:
                    if(get_key(V[N00(opcode)]) != true)
                        PC += 2;
                    break;
                default:
                    unknown(opcode);
                    break;
            }
            break;
        case 0xF:
            switch(NN(opcode))
            {
                case 0x07:
                    V[N00(opcode)] = get_timer(0);
                    break;
                case 0x0A:
                    key_found = false;
                    for(uint32_t i = 0; i <= 0xF && !key_found; i++)
                        if(key[i])  V[N00(opcode)] = i, key_found = true;

                    PC -= 2 * (!key_found);
                    break;
                case 0x15:
                    set_timer(0, V[N00(opcode)]);
                    break;
                case 0x18:
                    set_timer(1, V[N00(opcode)]);
                    break;
                case 0x1E:
                    I += V[N00(opcode)];
                    V[0xF] = (I >= ram_size);
                    break;
                case 0x29:
                    I = 5 * V[N00(opcode)];
                    break;
                case 0x33:
                    ram[I] = (V[N00(opcode)] / 100) % 10;
                    ram[I + 1] = (V[N00(opcode)] / 10) % 10;
                    ram[I + 2] = V[N00(opcode)] % 10;
                    break;
                case 0x55:
                    for(int i = 0; i <= N00(opcode); i++)
                        ram[I + i] = V[i];
                    break;
                case 0x65:
                    for(int i = 0; i <= N00(opcode); i++)
                        V[i] = ram[I + i];
                    break;
                case 0x75:
                    for(int i = 0; i <= N00(opcode); i++)
                        user_flags[i] = V[i];                    
                    break;
                case 0x85:
                    for(int i = 0; i <= N00(opcode); i++)
                        V[i] = user_flags[i];                    
                    break;                
                default:
                    unknown(opcode);
                    break;
            }
            break;
        default:
            unknown(opcode);
            break;
    }
}

void CHIP8_VM::unknown(uint16_t opcode)
{
    printf("\033[1;31m{E}: Unknown opcode: %04X \033[0m(%s)\n", opcode, disassembly(opcode).c_str());
    state = 3;    
}

std::string CHIP8_VM::disassembly(uint16_t opcode)
{
    std::stringstream result;
    // Hex is converted to uppercase, NOTHING ELSE is affected
    result << std::hex << std::uppercase;
    result << "\033[1;33m";
    switch(N000(opcode))
    {
        case 0x0:
            switch(NN(opcode))
            {
                case 0xE0:
                    result << "CLS";
                    break;
                case 0xEE:
                    result << "RET";
                    break;
                case 0xFF:
                    result << "HIGH";
                    break;
                default:
                    result << "Unknown disassembly!";
                    break;
            }
            break;
        case 0x1:
            result << "JP " << NNN(opcode);
            break;
        case 0x2:
            result << "CALL " << NNN(opcode);
            break;
        case 0x3:
            result << "SE V" << N00(opcode) << ", " << NN(opcode);
            break;
        case 0x4:
            result << "SNE V" << N00(opcode) << ", " << NN(opcode);
            break;
        case 0x5:
            result << "SE V" << N00(opcode) << ", V" << N0(opcode);
            break;
        case 0x6:
            result << "LD V" << N00(opcode) << ", " << NN(opcode);
            break;
        case 0x7:
            result << "ADD V" << N00(opcode) << ", " << NN(opcode);
            break;
        case 0x8:
            switch(N(opcode))
            {
                case 0x0:
                    result << "LD V" << N00(opcode) << ", V" << N0(opcode);
                    break;
                case 0x1:
                    result << "OR V" << N00(opcode) << ", V" << N0(opcode);
                    break;
                case 0x2:
                    result << "AND V" << N00(opcode) << ", V" << N0(opcode);
                    break;
                case 0x3:
                    result << "XOR V" << N00(opcode) << ", V" << N0(opcode);
                    break;
                case 0x4:
                    result << "ADD V" << N00(opcode) << ", V" << N0(opcode);
                    break;
                case 0x5:
                    result << "SUB V" << N00(opcode) << ", V" << N0(opcode);
                    break;
                case 0x6:
                    result << "SHR V" << N00(opcode);
                    break;
                case 0x7:
                    result << "SUBN V" << N00(opcode) << ", V" << N0(opcode);
                    break;
                case 0xE:
                    result << "SHL V" << N00(opcode);
                    break;
                default:
                    result << "Unknown disassembly!";
                    break;
            }
            break;
        case 0x9:
            result << "SNE V" << N00(opcode) << ", V" << N0(opcode);
            break;
        case 0xA:
            result << "LD I, " << NNN(opcode);
            break;
        case 0xB:
            result << "JP V0, " << NNN(opcode);
            break;
        case 0xC:
            result << "RND V" << N00(opcode) << ", " << NN(opcode);
            break;
        case 0xD:
            result << "DRW V" << N00(opcode) << ", V" << N0(opcode) << ", " << N(opcode);
            break;
        case 0xE:
            switch(NN(opcode))
            {
                case 0x9E:
                    result << "SKP V" << N00(opcode);
                    break;
                case 0xA1:
                    result << "SKNP V" << N00(opcode);
                    break;
                default:
                    result << "Unknown disassembly!";
                    break;
            }
            break;
        case 0xF:
            switch(NN(opcode))
            {
                case 0x07:
                    result << "LD V" << N00(opcode) << ", DT";
                    break;
                case 0x0A:
                    result << "LD V" << N00(opcode) << ", K";
                    break;
                case 0x15:
                    result << "LD DT, V" << N00(opcode);
                    break;
                case 0x18:
                    result << "LD ST, V" << N00(opcode);
                    break;
                case 0x1E:
                    result << "ADD I, V" << N00(opcode);
                    break;
                case 0x29:
                    result << "LD F, V" << N00(opcode);
                    break;
                case 0x30:
                    result << "LD F(S-8), V" << N00(opcode);
                    break;
                case 0x33:
                    result << "LD B, V" << N00(opcode);
                    break;
                case 0x55:
                    result << "LD [I], V" << N00(opcode);
                    break;
                case 0x65:
                    result << "LD V" << N00(opcode) << ", [I]";
                    break;
                case 0x75:
                    result << "LD R, V" << N00(opcode);
                    break;
                case 0x85:
                    result << "LD V" << N00(opcode) << ", R";
                    break;
                default:
                    result << "Unknown disassembly!";
                    break;
            }
            break;
        default:
            result << "Unknown disassembly!";
            break;
    }
    result << "\033[0m";
    return result.str();
}

void CHIP8_VM::updateKeys(Window* win)
{
    // Update keys
    for(uint32_t i = 0; i < 0x10; i++)
        key[i] = win -> getKey(keys[i]);
}

void CHIP8_VM::update(double dt)
{
    // Do not execute more than one second of delay (1 fps)
    dt = std::min(dt, 1.0);

    // If VM is still running
    if(state == 1)
    {
        // Update timers
        deltaTimer += dt;
        execTimer += dt;

        // Decrement timers
        while(deltaTimer >= 1.0 / 60)
        {
            if(timers[0] > 0)
                timers[0]--;
            if(timers[1] > 0)
                timers[1]--;
            deltaTimer -= 1.0 / 60;
        }

        // Execution loop
        while(execTimer >= 1.0 / frequency)
        {
            // Fetch and move forward
            opcode = (ram[PC] << 8) | ram[PC + 1];
            PC += 2;

            // Execute the opcode
            execute(opcode);

            // If out of bounds then exit
            if(PC >= ram_size || PC < rom_start)
                state = 2;
            
            // Decrement the timer
            execTimer -= 1.0 / frequency;
        }
    }
}

uint8_t CHIP8_VM::read(uint16_t offset)
{
    // Return value at offset
    return ram[offset];
}

void CHIP8_VM::write(uint16_t offset, uint8_t value)
{
    // Set value at offset
    ram[offset] = value;
}

int8_t CHIP8_VM::writeBuffer(uint16_t offset, uint8_t* buffer, uint32_t size)
{
    // Safety checks
    if(offset + size - 1 > ram_size)
    {
        printf("{E}: File size is too big! Maximum is %d, got %d!\n", ram_size, offset + size - 1);
        return -1;
    }

    // memcpy the values from the buffer to the RAM
    memcpy(ram + offset, buffer, size);

    // Success
    return 0;
}