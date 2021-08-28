#include "cpu.hpp"

namespace Gameboy
{
    // HL modifiers in the instructions requiring it
    static const int HL_ADD[] = {0, 0, 1, -1};

    // Macros for better usage
    // https://github.com/marethyu/noufu/blob/main/src/CPUOpcodes.cpp
    #define HC8(A, B, cy) ((((A) & 0x0F) + ((B) & 0x0F) + (cy)) > 0x0F)
    #define HC_ADD(A, B, cy) ((((A) & 0x0F) + ((B) & 0x0F) + (cy)) > 0x0F)
    #define HC_SUB(A, B, cy) ((((A) & 0x0F) - ((B) & 0x0F) - (cy)) < 0x00)
    #define CARRY(A, B, cy) ((((A) & 0xFF) + ((B) & 0xFF) + (cy)) >= 0x100)
    #define CARRY_SUB(A, B, cy) ((((A) & 0xFF) - ((B) & 0xFF) - (cy)) < 0x00)

    CPU::CPU()
    {
        // Initialise everything
        waitTimer = 0;

        // PC = 0x000 for normal boot, 0x100 for skipping the bios
        PC = 0x000;

        // Clear the registers with 0
        memset(reg8, 0, 10);

        // If skipping BIOS
        if(PC == 0x100)
        {
            reg8[0] = 0x13;
            reg8[1] = 0xFF;
            reg8[2] = 0xC1;
            reg8[3] = 0x00;
            reg8[4] = 0x03;
            reg8[5] = 0x84;
            reg8[6] = 0x00;
            reg8[7] = 0x01;
            reg8[8] = 0xFE;
            reg8[9] = 0xFF;
        }
    }

    void CPU::unknown_cb()
    {
        printf("\033[1;31m{E}: Unknown CB instruction byte: %02X at PC: %04X \033[0m\n", opcode, oldPC);
        exit(0);
    }

    void CPU::unknown()
    {
        printf("\033[1;31m{E}: Unknown instruction byte: %02X at PC: %04X \033[0m\n", opcode, oldPC);
        exit(0);
    }

    void CPU::rotate_left(uint8_t &n, bool circular, bool rZ = false)
    {
        bool flag = get_bit(n, 7);
        n <<= 1;
        if(circular ? flag : get_flag(4)) set_bit(n, 0, 1);
        set_flag(7, rZ ? 0 : n == 0);
        set_flag(6, 0);
        set_flag(5, 0);
        set_flag(4, flag);
    }

    uint8_t CPU::get_byte(uint16_t where)
    {
        // Update timer
        waitTimer += 4;
        
        return bus -> readByte(where);
    }

    void CPU::call(uint16_t addr)
    {
        // Push current PC to stack 
        push(PC);

        // Jump
        PC = addr;
    }

    void CPU::push(uint16_t data)
    {
        // Technically a store word but it's better like this
        write_byte(--SP, data >> 8);
        write_byte(--SP, data & 0xFF);
    }

    uint16_t CPU::pop()
    {
        uint16_t data = get_word(SP);
        SP += 2;
        return data;
    }

    uint16_t CPU::get_word(uint16_t where)
    {
        // Update timer
        waitTimer += 8;
        
        return bus -> readWord(where);
    }

    void CPU::write_byte(uint16_t where, uint8_t what)
    {
        // Update timer
        waitTimer += 4;
        
        bus -> writeByte(where, what);
    }

    void CPU::write_word(uint16_t where, uint16_t what)
    {
        // Update timer
        waitTimer += 8;

        bus -> writeWord(where, what);
    }

    void CPU::set_flag(uint8_t who, uint8_t what)
    {
        F = (F & ~(1 << who)) | (what << who);
    }

    bool CPU::get_condition(uint8_t what)
    {
        bool result = false;
        
        switch(what)
        {
            case 0: result = !get_flag(7); break;
            case 1: result =  get_flag(7); break;
            case 2: result = !get_flag(4); break;
            case 3: result =  get_flag(4); break;
        }

        if(result)  waitTimer += 4;

        return result;
    }

    void CPU::set_bit(uint8_t &who, uint8_t which, uint8_t what)
    {
        if(what)
            who |= (1 << which);
        else
            who &= ~(1 << which);
    }

    uint8_t CPU::get_bit(uint8_t who, uint8_t which)
    {
        return ((who >> which) & 1);
    }

    uint8_t CPU::get_flag(uint8_t who)
    {
        return get_bit(F, who);
    }

    void CPU::execute()
    {
        // If allowed to go
        if(waitTimer == 0)
        {
            oldPC = PC;
            opcode = get_byte(PC++);

            switch(opcode)
            {
            // NOP - do nothing
            case 0x00:  break;

            // XOR A with 8 bit register
            case 0xAF:
                A ^= *reg8_group[opcode - 0xA8];

                // Reset all flags and set the good one
                F = 0;
                set_flag(7, A == 0);

                break;

            // Store A at 16 bit register address - group 2
            case 0x22: case 0x32:
                // Store A
                write_byte(*reg16_group2[(opcode - 0x02) >> 4], A);

                // If HL needs to be updated
                HL += HL_ADD[(opcode - 0x02) >> 4];
                break;
            
            // Increment 8 bit register
            case 0x04: case 0x0C: case 0x24:
                set_flag(5, HC_ADD(*reg8_group[(opcode - 0x04) >> 3], 1, 0));
                *reg8_group[(opcode - 0x04) >> 3] += 1;
                set_flag(7, *reg8_group[(opcode - 0x04) >> 3] == 0);
                set_flag(6, 0);
                break;

            // Decrement 8 bit register
            case 0x05: case 0x0D: case 0x15: case 0x1D: case 0x3D:
                set_flag(5, HC_SUB(*reg8_group[(opcode - 0x05) >> 3], 1, 0));
                *reg8_group[(opcode - 0x05) >> 3] -= 1;
                set_flag(7, *reg8_group[(opcode - 0x05) >> 3] == 0);
                set_flag(6, 1);            
                break;

            // Load in 8 bit register
            case 0x06: case 0x0E: case 0x16: case 0x1E: case 0x2E: case 0x3E:
                *reg8_group[(opcode - 6) >> 3] = get_byte(PC++);
                break;

            // Load in 8 bit register from 8 bit register
            case 0x4F: // Load in C
            case 0x57: // Load in D
            case 0x67: // Load in H
            case 0x78: case 0x7B: case 0x7C: case 0x7D: // Load in A
                *reg8_group[opcode >> 3 & 7] = *reg8_group[opcode & 7];
                break;

            // Increment 16 bit register - group 1
            case 0x13: case 0x23:
                *reg16_group1[(opcode - 0x03) >> 4] += 1;
                waitTimer += 4;
                break;

            // Load in 16 bit register - group 1
            case 0x11: case 0x21: case 0x31:
                *reg16_group1[(opcode - 0x01) >> 4] = get_word(PC);
                PC += 2;
                break;

            // Load A from adress in 16 bit register - group 2
            case 0x1A:
                A = get_byte(*reg16_group2[(opcode - 0x0A) >> 4]);
                HL += HL_ADD[(opcode - 0x0A) >> 4];
                break;

            // Store A relative to 0xFF00 with C offset
            case 0xE2:
                write_byte(0xFF00 + C, A);
                break;

            // Store A relative to 0xFF00 with byte offset
            case 0xE0:
                write_byte(0xFF00 + get_byte(PC++), A);
                break;

            // Load A relative to 0xFF00 with byte offset
            case 0xF0:
                A = get_byte(0xFF00 + get_byte(PC++));
                break;

            // Store A at adress from memory
            case 0xEA:
                write_byte(get_word(PC), A);
                PC += 2;
                break;

            // Store register at adress (HL)
            case 0x77:
                write_byte(HL, *reg8_group[opcode - 0x70]);
                break;

            // Subtract from A
            case 0x90:
            {
                uint8_t reg = *reg8_group[opcode - (opcode <= 0x97 ? 0x90 : 0x98)];
                int cy = opcode > 0x97 ? get_flag(4) : 0;
                set_flag(5, HC_SUB(A, reg, cy));
                set_flag(4, CARRY_SUB(A, reg, cy));
                A -= reg + cy;
                set_flag(7, A == 0);
                set_flag(6, 1);
                break;
            }

            // Relative conditioned jump
            case 0x20: case 0x28:
            {
                int8_t offset = get_byte(PC++);
                if(get_condition((opcode - 0x20) >> 3))
                    PC += offset;
                break;
            }

            // Relative jump
            case 0x18:
            {
                int8_t offset = get_byte(PC++);
                PC += offset;
                break;
            }

            // Add to A byte at adress HL
            case 0x86:
            {
                uint8_t hl = get_byte(HL);
                int cy = opcode == 0x8E ? get_flag(4) : 0;
                set_flag(5, HC8(A, hl, cy));
                set_flag(4, CARRY(A, hl, cy));
                A += hl + cy;
                set_flag(7, A == 0);
                set_flag(6, 0);
                break;
            }

            // Compare A with byte
            case 0xFE:
            {
                uint8_t n = get_byte(PC++);
                set_flag(7, A == n);
                set_flag(6, 1);
                set_flag(5, HC_SUB(A, n, 0));
                set_flag(4, CARRY_SUB(A, n, 0));
                break;
            }

            // Compare A with byte at adress HL
            case 0xBE:
            {
                uint8_t n = get_byte(HL);
                set_flag(7, A == n);
                set_flag(6, 1);
                set_flag(5, HC_SUB(A, n, 0));
                set_flag(4, CARRY_SUB(A, n, 0));
                break;
            }

            // Call function at adress
            case 0xCD:
                PC += 2;
                call(get_word(PC - 2));
                waitTimer += 4;
                break;

            // Return from function
            case 0xC9:
                PC = pop();
                waitTimer += 4;
                break;
            
            // Push 16 bit register - group 3
            case 0xC5:
                push(*reg16_group3[(opcode - 0xC5) >> 4]);
                waitTimer += 4;
                break;

            // Pop 16 bit register - group 3
            case 0xC1:
                *reg16_group3[(opcode - 0xC1) >> 4] = pop();
                // Lower nibble of the flag register should stay untouched after popping
                if(opcode == 0xF1) F &= 0xF0;
                break;

            // Rotate bits left (on A)
            case 0x17:
                rotate_left(A, opcode == 0x07, true);
                break;

            // Special opcode (executes extended table)
            case 0xCB:
                // Fetch opcode part 2
                opcode = get_byte(PC++);

                switch(opcode)
                {
                // Test bit of 8 bit register
                case 0x7C:  // Bit 7
                    set_flag(7, !get_bit(*reg8_group[opcode & 7], opcode >> 3 & 7));
                    set_flag(6, 0);
                    set_flag(5, 1);                         
                    break;
                
                // Rotate bits left
                case 0x11:  // Without carry
                    rotate_left(*reg8_group[opcode & 7], !((opcode >> 3) & 7));
                    break;
                
                // Unknown CB opcode
                default:
                    unknown_cb();
                    break;
                }
                break;

            // Unknown opcode
            default:
                unknown();
                break;
            }
        }
        // Sleep
        else {  waitTimer--;  }
    }

    void CPU::attachBus(Bus* newBus)
    {
        bus = newBus;
    }
}
