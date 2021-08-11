#include "cpu.h"

Gameboy_CPU::Gameboy_CPU()
{
    // Initialise everything
    executionTimer = 0;
    waitTimer = 0;
    PC = 0x0000;
    reg_a = reg_b = reg_c = reg_d = reg_e = reg_f = reg_h = reg_l = 0;
}

void Gameboy_CPU::unknown()
{
    printf("\033[1;31m{E}: Unknown instruction byte: %02X \033[0m(%s)\n", instr_byte, dissasembly(instr_byte).c_str());
    exit(0);
}

std::string Gameboy_CPU::dissasembly(uint8_t instr_byte)
{
    std::stringstream result;
    // Hex is converted to uppercase, NOTHING ELSE is affected
    result << std::hex << std::uppercase;
    result << "\033[1;33m";
    switch(instr_byte)
    {
        case 0x21:
            result << "LD HL, " << bus -> readWord(PC + 1);
            break;
        case 0x31:
            result << "LD SP, " << bus -> readWord(PC + 1);
            break;
        case 0x32:
            result << "LD (HL-), A";
            break;
        case 0xAF:
            result << "XOR A, A";
            break;
        case 0xCB:
            result << "PREFIX CB";
            break;
        default:
            result << "Unknown dissasembly!";
            break;
    }
    result << "\033[0m";
    return result.str();
}

void Gameboy_CPU::set_flag(uint8_t who, uint8_t what)
{
    reg_f |= (what << who);
}

uint8_t Gameboy_CPU::get_flag(uint8_t who)
{
    return reg_f & (1 << who);
}

void Gameboy_CPU::op_xor(uint8_t &a, uint8_t b)
{
    a ^= b;
    reg_f = 0;
    set_flag(7, (a == 0));
}

void Gameboy_CPU::execute()
{
    // If allowed to go
    if(waitTimer == 0)
    {
        if(can_execute == false)
        {
            // Fetch takes 4 ticks, one is consumed on this if statement
            waitTimer = 3;
            can_execute = true;
        }
        else
        {
            instr_byte = bus -> readByte(PC);
            can_execute = false;
    
            switch(instr_byte)
            {
                case 0x21:
                    set_hl(bus -> readWord(PC + 1));
                    waitTimer += 8;
                    PC += 3;
                    break;
                case 0x31:
                    SP = bus -> readWord(PC + 1);
                    waitTimer += 8;
                    PC += 3;
                    break;
                case 0x32:
                    bus -> writeWord(get_hl(), reg_a);
                    set_hl(get_hl() - 1);
                    waitTimer += 4;
                    PC += 1;
                    break;
                case 0xAF:
                    op_xor(reg_a, reg_a);
                    PC += 1;
                    break;
                default:
                    unknown();
                    break;
            }
        }
    }
    // Sleep
    else {  waitTimer--;  }
}

void Gameboy_CPU::update(double dt)
{
    executionTimer += dt;

    while(executionTimer >= 1.0 / freq)
    {
        execute();
        executionTimer -= 1.0 / freq;
    }
}

void Gameboy_CPU::attachBus(Gameboy_Bus* newBus)
{
    bus = newBus;
}

uint16_t Gameboy_CPU::get_af()
{
    return (reg_a << 8) | reg_f;
}

void Gameboy_CPU::set_af(uint16_t val)
{
    reg_a = (val & 0xFF00) >> 8;
    reg_f = val & 0xFF;
}

uint16_t Gameboy_CPU::get_bc()
{
    return (reg_b << 8) | reg_c;
}

void Gameboy_CPU::set_bc(uint16_t val)
{
    reg_b = (val & 0xFF00) >> 8;
    reg_c = val & 0xFF;
}

uint16_t Gameboy_CPU::get_de()
{
    return (reg_d << 8) | reg_e;
}

void Gameboy_CPU::set_de(uint16_t val)
{
    reg_d = (val & 0xFF00) >> 8;
    reg_e = val & 0xFF;
}

uint16_t Gameboy_CPU::get_hl()
{
    return (reg_h << 8) | reg_l;
}

void Gameboy_CPU::set_hl(uint16_t val)
{
    reg_h = (val & 0xFF00) >> 8;
    reg_l = val & 0xFF;
}