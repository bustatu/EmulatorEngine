#include "vm.h"

#define N000(opcode) ((opcode & 0xF000) >> 12)
#define NNN(opcode) (opcode & 0xFFF)
#define N00(opcode) ((opcode & 0xF00) >> 8)
#define NN(opcode) (opcode & 0xFF)
#define N0(opcode) ((opcode & 0xF0) >> 4)
#define N(opcode) (opcode & 0xF)

namespace CHIP8
{
    VM::VM()
    {
        // Allocate RAM, registers, keys and stack
        ram = new uint8_t[ram_size];
        V = new uint8_t[0x10];
        stack = new uint16_t[stack_size];
        user_flags = new uint8_t[0x10];

        // Set the default screen size
        graphics.resize(64, 32);

        // Reset timers
        deltaTimer = execTimer = 0;

        // Stopped
        state = 0;

        // Set random seed time
        srand(time(NULL));
    }

    std::pair<int, int> VM::getSize()
    {
        return graphics.getSize();
    }

    void VM::setForeground(SDL_Color color)
    {
        graphics.setForegroundColor(color);
    }

    void VM::setBackground(SDL_Color color)
    {
        graphics.setBackgroundColor(color);
    }

    VM::~VM()
    {
        // Free RAM
        delete ram;
    }

    uint16_t VM::get_rom_start()
    {
        // Return rom_start
        return rom_start;
    }

    void VM::draw(SDL_Texture* &target, SDL_Renderer* tool)
    {
        graphics.draw(target, tool);
    }

    uint8_t VM::get_state()
    {
        // Return state
        return state;
    }

    void VM::set_rom_start(uint16_t offset)
    {
        // Set ROM start to offset
        rom_start = offset;
    }

    void VM::set_reg(uint8_t who, uint8_t what)
    {
        // Set register who to what
        V[who] = what;
    }

    void VM::set_I(uint16_t what)
    {
        // Set register I to what
        I = what;
    }

    void VM::boot()
    {
        // Clear everything
        memset(stack, 0, stack_size);
        memset(ram, 0, ram_size);
        memset(V, 0, 0x10);
        memset(user_flags, 0, 0x10);
        input.resetKeys();
        graphics.clear();

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

        // Load high-res font
        uint8_t highFont[] = {
            0xff, 0xff, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xff, 0xff, // 0
            0x0c, 0x0c, 0x3c, 0x3c, 0x0c, 0x0c, 0x0c, 0x0c, 0x3f, 0x3f, // 1
            0xff, 0xff, 0x03, 0x03, 0xff, 0xff, 0xc0, 0xc0, 0xff, 0xff, // 2
            0xff, 0xff, 0x03, 0x03, 0xff, 0xff, 0x03, 0x03, 0xff, 0xff, // 3
            0xc3, 0xc3, 0xc3, 0xc3, 0xff, 0xff, 0x03, 0x03, 0x03, 0x03, // 4
            0xff, 0xff, 0xc0, 0xc0, 0xff, 0xff, 0x03, 0x03, 0xff, 0xff, // 5
            0xff, 0xff, 0xc0, 0xc0, 0xff, 0xff, 0xc3, 0xc3, 0xff, 0xff, // 6
            0xff, 0xff, 0x03, 0x03, 0x0c, 0x0c, 0x30, 0x30, 0x30, 0x30, // 7
            0xff, 0xff, 0xc3, 0xc3, 0xff, 0xff, 0xc3, 0xc3, 0xff, 0xff, // 8
            0xff, 0xff, 0xc3, 0xc3, 0xff, 0xff, 0x03, 0x03, 0xff, 0xff  // 9
        };
        memcpy(ram + 5 * 16, highFont, 10 * 10);

        // Set PC to the start of the ROM
        PC = rom_start;

        // Clear stack
        sp = 0;
        
        // Clear timers (0 - delay, 1 - sound)
        timers[0] = timers[1] = 0;

        // Running
        state = 1;
    }

    // Draws sprite at position (va, vb) that has the height of vc
    void VM::DRW(uint8_t va, uint8_t vb, uint8_t vc)
    {
        uint16_t pixel;
        uint8_t offset = 1;
        std::pair<uint8_t, uint8_t> size = graphics.getSize();

        // 0 height means 16 height
        if(vc == 0)
        {
            vc = 16;
        
            // If in SCHIP8, sprites are double the normal size
            if(graphics.getState() == 1)
                offset = 2;
        }

        //Set the F register to 0
        V[0xF] = 0;

        //Go trough the sprite lines
        for(int32_t yline = 0; yline < vc; yline++)
        {
            
            //Read pixel from memory
            pixel = (ram[I + offset * yline] << 8) | ram[I + offset * yline + 1];

            //Go trough the sprite columns
            for(int32_t xline = 0; xline < 8 * offset; xline++)
                //If the pixel needs to be set
                if((pixel & ((1 << 15) >> xline)) != 0)
                    // Update the pixel and update V[0xF] if necessary
                    V[0xF] = graphics.xorPixel(va + xline + ((vb + yline) * size.first)) % (size.first * size.second) ? 1 : V[0xF];
        }
    }

    void VM::set_timer(uint8_t who, uint8_t what)
    {
        timers[who] = what;
    }

    uint8_t VM::get_timer(uint8_t who)
    {
        return timers[who];
    }

    void VM::execute(uint16_t opcode)
    {
        switch(N000(opcode))
        {
            case 0x0:
                switch(NN(opcode))
                {
                    case 0xE0:
                        graphics.clear();
                        break;
                    case 0xEE:
                        PC = stack[sp--];
                        break;
                    case 0xFF:
                        graphics.resize(128, 64);
                        graphics.setState(1);
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
                V[N00(opcode)] = NN(opcode);
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
                DRW(V[N00(opcode)], V[N0(opcode)], N(opcode));
                break;
            case 0xE:
                switch(NN(opcode))
                {
                    case 0x9E:
                        if(input.getKey(V[N00(opcode)]))
                            PC += 2;
                        break;
                    case 0xA1:
                        if(!input.getKey(V[N00(opcode)]))
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
                        if(input.getKeyCounter() != 0)
                            V[N00(opcode)] = input.getLastPressed();
                        PC -= 2 * (input.getKeyCounter() == 0);
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
                    case 0x30:
                        I = 5 * 16 + 10 * V[N00(opcode)];
                        break;
                    case 0x33:
                        ram[I] = (V[N00(opcode)] / 100) % 10;
                        ram[I + 1] = (V[N00(opcode)] / 10) % 10;
                        ram[I + 2] = V[N00(opcode)] % 10;
                        break;
                    case 0x55:
                        memcpy(ram + I, V, N00(opcode) + 1);
                        break;
                    case 0x65:
                        memcpy(V, ram + I, N00(opcode) + 1);
                        break;
                    case 0x75:
                        memcpy(user_flags, V, N00(opcode) + 1);           
                        break;
                    case 0x85:
                        memcpy(V, user_flags, N00(opcode) + 1);
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

    void VM::unknown(uint16_t opcode)
    {
        printf("\033[1;31m{E}: Unknown opcode: %04X \033[0m(%s)\n", opcode, disassembly(opcode).c_str());
        state = 3;    
    }

    std::string VM::disassembly(uint16_t opcode)
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
                        result << "LD HF, V" << N00(opcode);
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

    void VM::updateKeys(Window* win)
    {
        // Update keys
        for(uint32_t i = 0; i < 0x10; i++)
            input.setKey(i, win -> getKey(input.getMappedKey(i)));
    }

    void VM::update(double dt)
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

    uint8_t VM::read(uint16_t offset)
    {
        // Return value at offset
        return ram[offset];
    }

    void VM::write(uint16_t offset, uint8_t value)
    {
        // Set value at offset
        ram[offset] = value;
    }

    int8_t VM::writeBuffer(uint16_t offset, uint8_t* buffer, uint32_t size)
    {
        // Safety checks
        if(offset + size - 1 > ram_size)
        {
            printf("\033[1;31m{E}: File size is too big! Maximum is %d, got %d!\n\033[0m", ram_size - offset + 1, size);
            return -1;
        }

        // memcpy the values from the buffer to the RAM
        memcpy(ram + offset, buffer, size);

        // Success
        return 0;
    }
}