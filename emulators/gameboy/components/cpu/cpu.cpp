#include "cpu.h"

/// Macros for better usability
#define op_push(val) \
    bus -> writeWord(SP - 2, val); \
    SP -= 2; \
    waitTimer += 12; \
    PC += 1;
#define op_pop(val) \
    val(bus -> readWord(SP)); \
    SP += 2; \
    waitTimer += 8; \
    PC += 1;
#define op_reg_dec(reg) \
    reg--; \
    set_flag(7, (reg == 0)); \
    set_flag(6, 1); \
    set_flag(5, (reg & 0b111) == 0b111); \
    PC += 1;

namespace Gameboy
{
    CPU::CPU()
    {
        // Initialise everything
        waitTimer = 0;

        // PC = 0x0000 for normal boot, 0x1000 for skipping the bios
        PC = 0x0000;    // Skips bios
        reg_a = reg_b = reg_c = reg_d = reg_e = reg_f = reg_h = reg_l = 0;
    }

    void CPU::unknown()
    {
        printf("\033[1;31m{E}: Unknown instruction byte: %02X at PC: %04X \033[0m(%s)\n", instr_byte, PC, dissasembly(instr_byte).c_str());
        exit(0);
    }

    std::string CPU::dissasembly(uint8_t instr_byte)
    {
        std::stringstream result;
        // Hex is converted to uppercase, NOTHING ELSE is affected
        result << std::hex << std::uppercase;
        result << "\033[1;33m";
        switch(instr_byte)
        {
            case 0x00:
                result << "NOP";
                break;
            case 0x01:
                result << "LD BC, " << bus -> readWord(PC + 1);
                break;
            case 0x04:
                result << "INC B";
                break;
            case 0x05:
                result << "DEC B";
                break;
            case 0x06:
                result << "LD B, " << (uint32_t)(bus -> readByte(PC + 1));
                break;
            case 0x0C:
                result << "INC C";
                break;
            case 0x0D:
                result << "DEC C";
                break;
            case 0x0E:
                result << "LD C, " << (uint32_t)(bus -> readByte(PC + 1));
                break;
            case 0x10:
                result << "STOP";
                break;
            case 0x11:
                result << "LD DE, " << bus -> readWord(PC + 1);
                break;
            case 0x13:
                result << "INC DE";
                break;
            case 0x15:
                result << "DEC D";
                break;
            case 0x17:
                result << "RLA";
                break;
            case 0x18:
                result << "JR " << PC + 2 + (int8_t)(bus -> readByte(PC + 1));
                break;
            case 0x1A:
                result << "LD A, (DE)";
                break;
            case 0x1D:
                result << "DEC E";
                break;
            case 0x1E:
                result << "LD E, " << (uint32_t)(bus -> readByte(PC + 1));
                break;
            case 0x20:
                result << "JR NZ, " << PC + 2 + (int8_t)(bus -> readByte(PC + 1));
                break;
            case 0x21:
                result << "LD HL, " << bus -> readWord(PC + 1);
                break;
            case 0x22:
                result << "LD (HL+), A";
                break;
            case 0x23:
                result << "INC HL";
                break;
            case 0x24:
                result << "INC H";
                break;
            case 0x28:
                result << "JR Z, " << PC + 2 + (int8_t)(bus -> readByte(PC + 1));
                break;
            case 0x2A:
                result << "LD A, (HL+)";
                break;
            case 0x2E:
                result << "LD L, " << (uint32_t)(bus -> readByte(PC + 1));
                break;
            case 0x31:
                result << "LD SP, " << bus -> readWord(PC + 1);
                break;
            case 0x32:
                result << "LD (HL-), A";
                break;
            case 0x3D:
                result << "DEC A";
                break;
            case 0x3E:
                result << "LD A, " << (uint32_t)(bus -> readByte(PC + 1));
                break;
            case 0x4F:
                result << "LD C, A";
                break;
            case 0x57:
                result << "LD D, A";
                break;
            case 0x67:
                result << "LD H, A";
                break;
            case 0x77:
                result << "LD (HL), A";
                break;
            case 0x7B:
                result << "LD A, E";
                break;
            case 0x7C:
                result << "LD A, H";
                break;
            case 0x7D:
                result << "LD A, L";
                break;
            case 0x90:
                result << "SUB A, B";
                break;
            case 0xAF:
                result << "XOR A, A";
                break;
            case 0xC1:
                result << "POP BC";
                break;
            case 0xC3:
                result << "JP " << bus -> readWord(PC + 1);
                break;
            case 0xC5:
                result << "PUSH BC";
                break;
            case 0xC9:
                result << "RET";
                break;
            case 0xCB:
                switch(bus -> readByte(PC + 1))
                {
                    case 0x11:
                        result << "RL C";
                        break;
                    case 0x7C:
                        result << "BIT 7, H";
                        break;
                    default:
                        result << "Unknown prefix CB opcode " << (uint32_t)(bus -> readByte(PC + 1));
                        break;
                }
                break;
            case 0xCD:
                result << "CALL " << bus -> readWord(PC + 1);
                break;
            case 0xE0:
                result << "LD (FF00 + " << (uint32_t)(bus -> readByte(PC + 1)) << "), A";
                break;
            case 0xE1:
                result << "POP HL";
                break;
            case 0xE2:
                result << "LD (FF00 + C), A";
                break;
            case 0xE5:
                result << "PUSH HL";
                break;
            case 0xEA:
                result << "LD (" << bus -> readWord(PC + 1) << "), A";
                break;
            case 0xF0:
                result << "LD A, (FF00 + " << (uint32_t)(bus -> readByte(PC + 1)) << ")";
                break;
            case 0xF1:
                result << "POP AF";
                break;
            case 0xF3:
                result << "DI";
                break;
            case 0xF5:
                result << "PUSH AF";
                break;
            case 0xFE:
                result << "CP A, " << (uint32_t)(bus -> readByte(PC + 1));
                break;
            default:
                result << "Unknown dissasembly!";
                break;
        }
        result << "\033[0m";
        return result.str();
    }

    void CPU::set_flag(uint8_t who, uint8_t what)
    {
        if(what == 1)
            reg_f |= (1 << who);
        else
            reg_f &= ~(1 << who);
    }

    uint8_t CPU::get_flag(uint8_t who)
    {
        return ((reg_f >> who) & 1);
    }

    void CPU::op_xor(uint8_t &a, uint8_t b)
    {
        a ^= b;
        reg_f = 0;
        set_flag(7, (a == 0));
    }

    void CPU::execute()
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
                printf("%s %04X\n", dissasembly(instr_byte).c_str(), PC);
                /*std::ofstream fout;
                fout.open("trace.log", std::ios_base::app);
                fout << dissasembly(instr_byte) << " " << PC << "\n";
                fout.close();*/
                can_execute = false;
                switch(instr_byte)
                {
                    case 0x00:
                        PC += 1;
                        break;
                    case 0x01:
                        set_bc(bus -> readWord(PC + 1));
                        waitTimer += 8;
                        PC += 3;                        
                        break;
                    case 0x04:
                        reg_b++;
                        set_flag(7, (reg_b == 0)); 
                        set_flag(6, 0);
                        set_flag(5, (reg_b & 0b111) == 0b000);                 
                        PC += 1;
                        break;
                    case 0x05:  op_reg_dec(reg_b);   break;
                    case 0x06:
                        reg_b = bus -> readByte(PC + 1);
                        waitTimer += 4;
                        PC += 2;
                        break;
                    case 0x0C:
                        reg_c++;
                        set_flag(7, (reg_c == 0)); 
                        set_flag(6, 0);
                        set_flag(5, (reg_c & 0b111) == 0b000);                 
                        PC += 1;
                        break;
                    case 0x0D:  op_reg_dec(reg_c);  break;
                    case 0x0E:
                        reg_c = bus -> readByte(PC + 1);
                        waitTimer += 4;
                        PC += 2;
                        break;
                    case 0x10:
                        // TODO: Find out what STOP does
                        PC += 1;
                        break;
                    case 0x11:
                        set_de(bus -> readWord(PC + 1));
                        waitTimer += 8;
                        PC += 3;
                        break;
                    case 0x13:
                        set_de(get_de() + 1);
                        waitTimer += 4;
                        PC += 1;
                        break;
                    case 0x15:  op_reg_dec(reg_d);  break;
                    case 0x17:
                        reg_f = 0;
                        set_flag(4, (reg_a & (1 << 7)) >> 7);
                        reg_a <<= 1;
                        reg_a |= get_flag(4);
                        PC += 1;
                        break;
                    case 0x18:
                        PC += 2 + (int8_t)(bus -> readByte(PC + 1)); 
                        waitTimer += 8;         
                        break;
                    case 0x1A:
                        reg_a = bus -> readByte(get_de());
                        waitTimer += 4;
                        PC += 1;
                        break;
                    case 0x1D:  op_reg_dec(reg_e);  break;
                    case 0x1E:
                        reg_e = bus -> readByte(PC + 1);
                        waitTimer += 4;
                        PC += 2;
                        break;
                    case 0x20:
                        if(!get_flag(7))
                        {
                            PC += (int8_t)(bus -> readByte(PC + 1)); 
                            waitTimer += 8;
                        }
                        else
                            waitTimer += 4;
                        PC += 2;
                        break;
                    case 0x21:
                        set_hl(bus -> readWord(PC + 1));
                        waitTimer += 8;
                        PC += 3;
                        break;
                    case 0x22:
                        bus -> writeWord(get_hl(), reg_a);
                        set_hl(get_hl() + 1);
                        waitTimer += 4;
                        PC += 1;                    
                        break;
                    case 0x23:
                        set_hl(get_hl() + 1);
                        waitTimer += 4;
                        PC += 1;
                        break;
                    case 0x24:
                        reg_h++;
                        set_flag(7, (reg_h == 0)); 
                        set_flag(6, 0);
                        set_flag(5, (reg_h & 0b111) == 0b000);                 
                        PC += 1;
                        break;
                    case 0x28:
                        if(get_flag(7))
                        {
                            PC += (int8_t)(bus -> readByte(PC + 1)); 
                            waitTimer += 8;
                        }
                        else
                            waitTimer += 4;
                        PC += 2;
                        break;
                    case 0x2A:
                        reg_a = bus -> readByte(get_hl());
                        set_hl(get_hl() + 1);
                        waitTimer += 4;
                        PC += 1;
                        break;
                    case 0x2E:
                        reg_l = bus -> readByte(PC + 1);
                        waitTimer += 4;
                        PC += 2;
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
                    case 0x3D:  op_reg_dec(reg_a);  break;
                    case 0x3E:
                        reg_a = bus -> readByte(PC + 1);
                        waitTimer += 4;
                        PC += 2;
                        break;
                    case 0x4F:
                        reg_c = reg_a;
                        PC += 1;
                        break;
                    case 0x57:
                        reg_d = reg_a;
                        PC += 1;
                        break;
                    case 0x67:
                        reg_h = reg_a;
                        PC += 1;
                        break;
                    case 0x77:
                        bus -> writeWord(get_hl(), reg_a);
                        waitTimer += 4;
                        PC += 1;
                        break;
                    case 0x7B:
                        reg_a = reg_e;
                        PC += 1;
                        break;
                    case 0x7C:
                        reg_a = reg_h;
                        PC += 1;
                        break;
                    case 0x7D:
                        reg_a = reg_l;
                        PC += 1;
                        break;
                    case 0x90:
                        reg_a -= reg_b;
                        set_flag(7, (reg_a == 0));
                        set_flag(6, 1);
                        set_flag(5, (reg_a & 0b111) < (reg_b & 0b111));
                        set_flag(4, reg_a  < reg_b);
                        PC += 1;
                        break;
                    case 0xAF:
                        reg_f = 0;
                        op_xor(reg_a, reg_a);
                        set_flag(7, reg_a == 0);
                        PC += 1;
                        break;
                    case 0xC1:  op_pop(set_bc); break;
                    case 0xC3:
                        PC = bus -> readWord(PC + 1);
                        waitTimer += 12;
                        break;
                    case 0xC5:  op_push(get_bc());  break;
                    case 0xC9:
                        PC = bus -> readWord(SP);
                        SP += 2;
                        waitTimer += 12;
                        break;
                    case 0xCB:
                        switch(bus -> readByte(PC + 1))
                        {
                            case 0x11:
                                reg_f = 0;
                                set_flag(4, (reg_c & (1 << 7)) >> 7);
                                reg_c <<= 1;
                                reg_c |= get_flag(4);
                                set_flag(7, reg_c == 0);
                                waitTimer += 4;
                                PC += 2;
                                break;
                            case 0x7C:
                                set_flag(7, ((reg_h >> 7) & 1) == 0);
                                set_flag(6, 0);
                                set_flag(5, 1);
                                waitTimer += 4;
                                PC += 2;
                                break;
                            default:
                                unknown();
                                break;
                        }
                        break;
                    case 0xCD:
                        bus -> writeWord(SP - 2, PC + 3);
                        SP -= 2;
                        PC = bus -> readWord(PC + 1);
                        waitTimer += 20;
                        break;
                    case 0xE0:
                        bus -> writeByte(0xFF00 + bus -> readByte(PC + 1), reg_a);
                        waitTimer += 8;
                        PC += 2;
                        break;
                    case 0xE1:  op_pop(set_hl); break;
                    case 0xE2:
                        bus -> writeByte(0xFF00 + reg_c, reg_a);
                        waitTimer += 4;
                        PC += 1;
                        break;
                    case 0xE5:  op_push(get_hl());  break;
                    case 0xEA:
                        bus -> writeByte(bus -> readWord(PC + 1), reg_a);
                        waitTimer += 12;
                        PC += 3;
                        break;
                    case 0xF0:
                        reg_a = bus -> readByte(0xFF00 + bus -> readByte(PC + 1));
                        waitTimer += 8;
                        PC += 2;
                        break;
                    case 0xF1:  op_pop(set_af); break;
                    case 0xF3:
                        ime_flag = false;
                        PC += 1;
                        break;
                    case 0xF5:  op_push(get_af());  break;
                    case 0xFE:
                        set_flag(7, reg_a == bus -> readByte(PC + 1));
                        set_flag(6, 1);
                        set_flag(5, (reg_a & 0xFF) < (bus -> readByte(PC + 1) & 0xFF));
                        set_flag(4, reg_a < bus -> readByte(PC + 1));
                        waitTimer += 4;
                        PC += 2;
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

    void CPU::attachBus(Bus* newBus)
    {
        bus = newBus;
    }

    uint16_t CPU::get_af()
    {
        return (reg_a << 8) | reg_f;
    }

    void CPU::set_af(uint16_t val)
    {
        reg_a = (val & 0xFF00) >> 8;
        reg_f = val & 0xFF;
    }

    uint16_t CPU::get_bc()
    {
        return (reg_b << 8) | reg_c;
    }

    void CPU::set_bc(uint16_t val)
    {
        reg_b = (val & 0xFF00) >> 8;
        reg_c = val & 0xFF;
    }

    uint16_t CPU::get_de()
    {
        return (reg_d << 8) | reg_e;
    }

    void CPU::set_de(uint16_t val)
    {
        reg_d = (val & 0xFF00) >> 8;
        reg_e = val & 0xFF;
    }

    uint16_t CPU::get_hl()
    {
        return (reg_h << 8) | reg_l;
    }

    void CPU::set_hl(uint16_t val)
    {
        reg_h = (val & 0xFF00) >> 8;
        reg_l = val & 0xFF;
    }
}
