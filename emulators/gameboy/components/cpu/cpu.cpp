#include "cpu.hpp"

namespace Gameboy
{
    // HL modifiers in the instructions requiring it
    static const int HL_ADD[] = {0, 0, 1, -1};

    // Reset adress for quick RST calls
    static const int RST_ADDR[] = {0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38};

    // Macros for better usage
    // https://github.com/marethyu/noufu/blob/main/src/CPUOpcodes.cpp
    #define HC8(A, B, cy) ((((A) & 0x0F) + ((B) & 0x0F) + (cy)) > 0x0F)
    #define HC16(A, B) ((((A) & 0xFFF) + ((B) & 0xFFF)) > 0xFFF)
    #define HC_ADD(A, B, cy) ((((A) & 0x0F) + ((B) & 0x0F) + (cy)) > 0x0F)
    #define HC_SUB(A, B, cy) ((((A) & 0x0F) - ((B) & 0x0F) - (cy)) < 0x00)
    #define CARRY(A, B, cy) ((((A) & 0xFF) + ((B) & 0xFF) + (cy)) >= 0x100)
    #define CARRY16(A, B) (((A) + (B)) >= 0x10000)
    #define CARRY_SUB(A, B, cy) ((((A) & 0xFF) - ((B) & 0xFF) - (cy)) < 0x00)
    #define SWAP_NIBBLES(n) n = ((n & 0x0F) << 4) | (n >> 4);

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
            bus -> writeByte(0xFF50, 1);
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

    void CPU::shift_right(uint8_t &n, bool logical)
    {
        bool flag0 = get_bit(n, 0), flag7 = get_bit(n, 7);
        n >>= 1;
        // The content of bit 7 is unchanged. (see the gameboy programming manual and the original emu refference I used)
        if (!logical && flag7) set_bit(n, 7, 1);
        set_flag(7, n == 0);
        set_flag(6, 0);
        set_flag(5, 0);
        set_flag(4, flag0);
    }

    void CPU::shift_left(uint8_t &n)
    {
        bool flag = get_bit(n, 7);
        n <<= 1;
        set_flag(7, n == 0);
        set_flag(6, 0);
        set_flag(5, 0);
        set_flag(4, flag);
    }

    void CPU::rotate_right(uint8_t &n, bool circular, bool rZ)
    {
        bool flag = get_bit(n, 0);
        n >>= 1;
        if(circular ? flag : get_flag(4)) set_bit(n, 7, 1);
        set_flag(7, rZ ? 0 : n == 0);
        set_flag(6, 0);
        set_flag(5, 0);
        set_flag(4, flag);
    }

    void CPU::rotate_left(uint8_t &n, bool circular, bool rZ)
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

            // Load in 8 bit register from 8 bit register
            case 0x40: case 0x41: case 0x42: case 0x43: case 0x44: case 0x45: case 0x47: // Load in B
            case 0x48: case 0x49: case 0x4A: case 0x4B: case 0x4C: case 0x4D: case 0x4F: // Load in C
            case 0x50: case 0x51: case 0x52: case 0x53: case 0x54: case 0x55: case 0x57: // Load in D
            case 0x58: case 0x59: case 0x5A: case 0x5B: case 0x5C: case 0x5D: case 0x5F: // Load in E
            case 0x60: case 0x61: case 0x62: case 0x63: case 0x64: case 0x65: case 0x67: // Load in H
            case 0x68: case 0x69: case 0x6A: case 0x6B: case 0x6C: case 0x6D: case 0x6F: // Load in L
            case 0x78: case 0x79: case 0x7A: case 0x7B: case 0x7C: case 0x7D: case 0x7F: // Load in A
                *reg8_group[opcode >> 3 & 7] = *reg8_group[opcode & 7];
                break;

            // Load in 8 bit register from PC
            case 0x06: case 0x0E: case 0x16: case 0x1E: case 0x26: case 0x2E: case 0x3E:
                *reg8_group[(opcode - 6) >> 3] = get_byte(PC++);
                break;

            // Load in 8 bit register value at adress HL
            case 0x46: case 0x4E: case 0x56: case 0x5E: case 0x66: case 0x6E: case 0x7E:
                *reg8_group[(opcode - 70) >> 3] = get_byte(HL);
                break;

            // Store 8 bit register at adress HL
            case 0x70: case 0x71: case 0x72: case 0x73: case 0x74: case 0x75: case 0x77:
                write_byte(HL, *reg8_group[opcode - 0x70]);
                break;
            
            // Load byte at adress HL
            case 0x36:
                write_byte(HL, get_byte(PC++));
                break;

            // Load A from adress in 16 bit register - group 2
            case 0x0A: case 0x1A: case 0x2A: case 0x3A:
                A = get_byte(*reg16_group2[(opcode - 0x0A) >> 4]);
                HL += HL_ADD[(opcode - 0x0A) >> 4];
                break;

            // Store A at 16 bit register address - group 2
            case 0x02: case 0x12: case 0x22: case 0x32:
                // Store A
                write_byte(*reg16_group2[(opcode - 0x02) >> 4], A);

                // If HL needs to be updated
                HL += HL_ADD[(opcode - 0x02) >> 4];
                break;

            // Load A from adress at next word
            case 0xFA:
                A = get_byte(get_word(PC));
                PC += 2;
                break;

            // Store A at adress from next word
            case 0xEA:
                write_byte(get_word(PC), A);
                PC += 2;
                break;

            // Load A relative to 0xFF00 with byte offset
            case 0xF0:
                A = get_byte(0xFF00 + get_byte(PC++));
                break;

            // Store A relative to 0xFF00 with byte offset
            case 0xE0:
                write_byte(0xFF00 + get_byte(PC++), A);
                break;

            // Load A relative to 0xFF00 with C offset
            case 0xF2:
                A = get_byte(0xFF00 + C);
                break;

            // Store A relative to 0xFF00 with C offset
            case 0xE2:
                write_byte(0xFF00 + C, A);
                break;

            // Load in 16 bit register - group 1
            case 0x01: case 0x11: case 0x21: case 0x31:
                *reg16_group1[(opcode - 0x01) >> 4] = get_word(PC);
                PC += 2;
                break;

            // Store stack pointer to adress from next word
            case 0x08:
                write_word(get_word(PC), SP);
                PC += 2;
                break;

            // Store HL in SP
            case 0xF9:
                SP = HL;
                waitTimer += 4;
                break;

            // Push 16 bit register - group 3
            case 0xC5: case 0xD5: case 0xE5: case 0xF5:
                push(*reg16_group3[(opcode - 0xC5) >> 4]);
                waitTimer += 4;
                break;

            // Pop 16 bit register - group 3
            case 0xC1: case 0xD1: case 0xE1: case 0xF1:
                *reg16_group3[(opcode - 0xC1) >> 4] = pop();
                // Lower nibble of the flag register should stay untouched after popping
                if(opcode == 0xF1) F &= 0xF0;
                break;

            // Add 8 bit register to A with / without carry
            case 0x80: case 0x81: case 0x82: case 0x83: case 0x84: case 0x85: case 0x87: // Without carry
            case 0x88: case 0x89: case 0x8A: case 0x8B: case 0x8C: case 0x8D: case 0x8F: // With carry
            {
                uint8_t reg = *reg8_group[opcode - (opcode <= 0x87 ? 0x80 : 0x88)];
                int cy = opcode > 0x87 ? get_flag(4) : 0;
                set_flag(5, HC8(A, reg, cy));
                set_flag(4, CARRY(A, reg, cy));
                A += reg + cy;
                set_flag(7, A == 0);
                set_flag(6, 0);
                break;
            }

            // Add next byte to A with / without carry
            case 0xC6: case 0xCE:   // Without / With
            {
                uint8_t n = get_byte(PC++);
                int cy = opcode == 0xCE ? get_flag(4) : 0;
                set_flag(5, HC8(A, n, cy));
                set_flag(4, CARRY(A, n, cy));
                A += n + cy;
                set_flag(7, A == 0);
                set_flag(6, 0);
                break;
            }

            // Add byte at adress HL to A with / without carry
            case 0x86: case 0x8E:   // Without / With
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

            // Subtract 8 bit register from A
            case 0x90: case 0x91: case 0x92: case 0x93: case 0x94: case 0x95: case 0x97: // Without carry
            case 0x98: case 0x99: case 0x9A: case 0x9B: case 0x9C: case 0x9D: case 0x9F: // With carry
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

            // Subtract next byte from A with / without carry
            case 0xD6: case 0xDE:   // Without / With
            {
                uint8_t n = get_byte(PC++);
                int cy = opcode == 0xDE ? get_flag(4) : 0;
                set_flag(5, HC_SUB(A, n, cy));
                set_flag(4, CARRY_SUB(A, n, cy));
                A -= n + cy;
                set_flag(7, A == 0);
                set_flag(6, 1);
                break;
            }

            // Subtract byte at adress HL from A with / without carry
            case 0x96: case 0x9E:   // Without / With
            {
                uint8_t hl = get_byte(HL);
                int cy = opcode == 0x9E ? get_flag(4) : 0;
                set_flag(5, HC_SUB(A, hl, cy));
                set_flag(4, CARRY_SUB(A, hl, cy));
                A -= hl + cy;
                set_flag(7, A == 0);
                set_flag(6, 1);
            }

            // AND A with 8 bit register
            case 0xA0: case 0xA1: case 0xA2: case 0xA3: case 0xA4: case 0xA5: case 0xA7:
                A &= *reg8_group[opcode - 0xA0];
                F = 0;
                set_flag(7, A == 0);
                set_flag(5, 1);
                break;

            // AND A with next byte
            case 0xE6:
                A &= get_byte(PC++);
                F = 0;
                set_flag(7, A == 0);
                set_flag(5, 1);
                break;

            // AND A with adress at HL
            case 0xA6:
                A &= get_byte(HL);
                F = 0;
                set_flag(7, A == 0);
                set_flag(4, 1);
                break;

            // XOR A with 8 bit register
            case 0xA8: case 0xA9: case 0xAA: case 0xAB: case 0xAC: case 0xAD: case 0xAF:
                A ^= *reg8_group[opcode - 0xA8];

                // Reset all flags and set the good one
                F = 0;
                set_flag(7, A == 0);
                break;

            // XOR A with next byte
            case 0xEE:
                A ^= get_byte(PC++);

                // Reset all flags and set the good one
                F = 0;
                set_flag(7, A == 0);
                break;

            // XOR A with byte at adress HL
            case 0xAE:
                A ^= get_byte(HL);

                // Reset all flags and set the good one
                F = 0;
                set_flag(7, A == 0);
                break;

            // OR A with 8 bit register
            case 0xB0: case 0xB1: case 0xB2: case 0xB3: case 0xB4: case 0xB5: case 0xB7:
                A |= *reg8_group[opcode - 0xB0];

                // Reset all flags and set the good one
                F = 0;
                set_flag(7, A == 0);
                break;

            // OR A with next byte
            case 0xF6:
                A |= get_byte(PC++);

                // Reset all flags and set the good one
                F = 0;
                set_flag(7, A == 0);
                break;

            // OR A with byte at adress HL
            case 0xB6:
                A |= get_byte(HL);

                // Reset all flags and set the good one
                F = 0;
                set_flag(7, A == 0);
                break;

            // Compare A with 8 bit register
            case 0xB8: case 0xB9: case 0xBA: case 0xBB: case 0xBC: case 0xBD: case 0xBF:
            {
                uint8_t reg = *reg8_group[opcode - 0xB8];
                set_flag(7, A == reg);
                set_flag(6, 1);
                set_flag(5, HC_SUB(A, reg, 0));
                set_flag(4, CARRY_SUB(A, reg, 0));
                break;
            }

            // Compare A with next byte
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
            
            // Increment 8 bit register
            case 0x04: case 0x0C: case 0x14: case 0x1C: case 0x24: case 0x2C: case 0x3C:
                set_flag(5, HC_ADD(*reg8_group[(opcode - 0x04) >> 3], 1, 0));
                *reg8_group[(opcode - 0x04) >> 3] += 1;
                set_flag(7, *reg8_group[(opcode - 0x04) >> 3] == 0);
                set_flag(6, 0);
                break;

            // Increment byte at adress HL
            case 0x34:
            {
                uint8_t hl = get_byte(HL);
                set_flag(5, HC8(hl, 1, 0));
                hl += 1;
                set_flag(7, hl == 0);
                write_byte(HL, hl);
                set_flag(6, 0);
                break;
            }

            // Decrement 8 bit register
            case 0x05: case 0x0D: case 0x15: case 0x1D: case 0x25: case 0x2D: case 0x3D:
                set_flag(5, HC_SUB(*reg8_group[(opcode - 0x05) >> 3], 1, 0));
                *reg8_group[(opcode - 0x05) >> 3] -= 1;
                set_flag(7, *reg8_group[(opcode - 0x05) >> 3] == 0);
                set_flag(6, 1);            
                break;

            // Decrement 8 bit value at adress HL
            case 0x35:
            {
                uint8_t hl = get_byte(HL);
                set_flag(5, HC_SUB(hl, 1, 0));
                hl -= 1;
                set_flag(7, hl == 0);
                write_byte(HL, hl);
                set_flag(6, 0);
                break;
            }

            // DAA - Decimal Adjust Accumulator - adjusts the accumulator for BCD operations
            // Why is this a thing?
            case 0x27:
            {
                uint16_t correction = 0, flagcarry = 0;

                // Correct based on digits and operation
                if(get_flag(5) || (!get_flag(6) && (A & 0xF) > 9))
                    correction |= 0x6;
                if(get_flag(4) || (!get_flag(6) && A > 0x99))
                    correction |= 0x60, flagcarry = 1;

                // Correct the acumulator
                A += get_flag(6) ? -correction : correction;
                A &= 0xFF;

                // Set zero flag
                set_flag(7, A == 0);
                set_flag(5, 0);
                set_flag(4, flagcarry);
                break;
            }

            // Complement A
            case 0x2F:
                A ^= 0xFF;
                set_flag(6, 1);
                set_flag(5, 1);
                break;

            // Add to HL a 16 bit register
            case 0x09: case 0x19: case 0x29: case 0x39:
            {
                uint16_t val = *reg16_group1[(opcode - 0x09) >> 4];
                set_flag(5, HC16(HL, val));
                set_flag(4, CARRY16(HL, val));
                HL += val;
                set_flag(6, 0);
                waitTimer += 4;
                break;
            }

            // Increment 16 bit register - group 1
            case 0x03: case 0x13: case 0x23: case 0x33:
                *reg16_group1[(opcode - 0x03) >> 4] += 1;
                waitTimer += 4;
                break;

            // Decrement 16 bit register - group 1
            case 0x0B: case 0x1B: case 0x2B: case 0x3B:
                *reg16_group1[(opcode - 0x0B) >> 4] -= 1;
                waitTimer += 4;
                break;

            // Add next signed byte to SP
            case 0xE8:
            {
                int8_t dd = get_byte(PC++);
                set_flag(5, HC8(SP, dd, 0));
                set_flag(4, CARRY(SP, dd, 0));
                SP += dd;
                set_flag(7, 0);
                set_flag(6, 0);
                waitTimer += 8;
                break;
            }
            
            // Set HL to SP + next signed byte
            case 0xF8:
            {
                int8_t dd = get_byte(PC++);
                set_flag(5, HC8(SP, dd, 0));
                set_flag(4, CARRY(SP, dd, 0));
                HL = SP + dd;
                set_flag(7, 0);
                set_flag(6, 0);
                waitTimer += 4;
                break;
            }

            // Rotate A's bits left
            case 0x07: case 0x17: // Without / with carry
                rotate_left(A, opcode == 0x07, true);
                break;

            // Rotate A's bits right
            case 0x0F: case 0x1F: // Without / with carry
                rotate_right(A, opcode == 0x0F, true);
                break;

            // Complement carry flag
            case 0x3F:
                set_flag(6, 0);
                set_flag(5, 0);
                set_flag(4, !get_flag(4));
                break;

            // Set carry flag
            case 0x37:
                set_flag(6, 0);
                set_flag(5, 0);
                set_flag(4, 1);
                break;

            // Halt
            case 0x76:
                // TODO: implement this
                printf("\033[1;31m{E}: Unhandled halt opcode!\033[0m\n");
                break;
            
            // Stop
            case 0x10:
                // TODO: implement this
                printf("\033[1;31m{E}: Unhandled stop opcode!\033[0m\n");
                break;

            // Disable interrupts
            case 0xF3:
                // TODO: implement this
                printf("\033[1;31m{E}: Unhandled disable interrupts opcode!\033[0m\n");
                break;

            // Enable interrupts
            case 0xFB:
                // TODO: implement this
                printf("\033[1;31m{E}: Unhandled enable interrupts opcode!\033[0m\n");
                break;

            // Direct jump
            case 0xC3:
                PC = get_word(PC);
                waitTimer += 4;
                break;

            // Jump to HL
            case 0xE9:
                PC = HL;
                break;

            // Conditioned jump to next byte
            case 0xC2: case 0xCA: case 0xD2: case 0xDA:
                if(get_condition((opcode - 0xC2) >> 3))
                    PC = get_word(PC);
                break;

            // Relative jump
            case 0x18:
            {
                int8_t offset = get_byte(PC++);
                PC += offset;
                break;
            }

            // Relative conditioned jump
            case 0x20: case 0x28: case 0x30: case 0x38:
            {
                int8_t offset = get_byte(PC++);
                if(get_condition((opcode - 0x20) >> 3))
                    PC += offset;
                break;
            }

            // Call function at next word adress
            case 0xCD:
                PC += 2;
                call(get_word(PC - 2));
                waitTimer += 4;
                break;

            // Call function conditional at next word adress
            case 0xC4: case 0xCC: case 0xD4: case 0xDC:
            {
                uint16_t nn = get_word(PC);
                PC += 2;
                if(get_condition((opcode - 0xC4) >> 3))
                    call(nn);
                break;
            }

            // Return from function
            case 0xC9:
                PC = pop();
                waitTimer += 4;
                break;

            // Conditional return from function
            case 0xC0: case 0xC8: case 0xD0: case 0xD8:
                if(get_condition((opcode - 0xC0) >> 3))
                    PC = pop();
                waitTimer += 4;
                break;

            // Return and enable interrupts
            case 0xD9:
                printf("\033[1;31m{E}: Unhandled return and enable interrupts opcode!\033[0m\n");
                break;

            // Call from special vector table
            case 0xC7: case 0xCF: case 0xD7: case 0xDF: case 0xE7: case 0xEF: case 0xF7: case 0xFF:
                call(RST_ADDR[(opcode - 0xC7) >> 3]);
                waitTimer += 4;
                break;

            // Special opcode (executes extended table)
            case 0xCB:
                // Fetch opcode part 2
                opcode = get_byte(PC++);

                switch(opcode)
                {
                // Rotate 8 bit register bits left
                case 0x00: case 0x01: case 0x02: case 0x03: case 0x04: case 0x05: case 0x07: // With carry
                case 0x10: case 0x11: case 0x12: case 0x13: case 0x14: case 0x15: case 0x17: // Without carry
                    rotate_left(*reg8_group[opcode & 7], !((opcode >> 3) & 7), false);
                    break;

                // Rotate byte at adress HL bits left
                case 0x06: case 0x16: // With / without carry
                {
                    uint8_t hl = get_byte(HL);
                    rotate_left(hl, opcode == 0x06, false);
                    write_byte(HL, hl);
                    break;
                }

                // Rotate 8 bit register bits right
                case 0x08: case 0x09: case 0x0A: case 0x0B: case 0x0C: case 0x0D: case 0x0F: // With carry
                case 0x18: case 0x19: case 0x1A: case 0x1B: case 0x1C: case 0x1D: case 0x1F: // Without carry
                    rotate_right(*reg8_group[(opcode & 0x0F) - 0x08], !((opcode >> 4) & 0x0F), false);
                    break;

                // Rotate byte at adress HL bits right
                case 0x0E: case 0x1E: // With / without carry
                {
                    uint8_t hl = get_byte(HL);
                    rotate_right(hl, opcode == 0x0E, false);
                    write_byte(HL, hl);
                    break;
                }

                // Shift 8 bit register bits left arithmetic
                case 0x20: case 0x21: case 0x22: case 0x23: case 0x24: case 0x25: case 0x27:
                    shift_left(*reg8_group[opcode - 0x38]);
                    break;

                // Shift byte at adress HL left arithmetic
                case 0x26:
                {
                    uint8_t hl = get_byte(HL);
                    shift_left(hl);
                    write_byte(HL, hl);
                    break;
                }

                // Swap 8 bit register nibbles
                case 0x30: case 0x31: case 0x32: case 0x33: case 0x34: case 0x35: case 0x37:
                {
                    uint8_t n = *reg8_group[opcode - 0x30];
                    SWAP_NIBBLES(n);
                    set_flag(7, n == 0);
                    set_flag(6, 0);
                    set_flag(5, 0);
                    set_flag(4, 0);
                    *reg8_group[opcode - 0x30] = n;
                    break;
                }

                // Swap byte at adress HL nibbles
                case 0x36:
                {
                    uint8_t hl = get_byte(HL);
                    SWAP_NIBBLES(hl);
                    set_flag(7, hl == 0);
                    set_flag(6, 0);
                    set_flag(5, 0);
                    set_flag(4, 0);
                    write_byte(HL, hl);
                    break;
                }

                // Shift 8 bit register bits right arithmetic
                case 0x28: case 0x29: case 0x2A: case 0x2B: case 0x2C: case 0x2D: case 0x2F:
                    shift_right(*reg8_group[opcode - 0x38], false);
                    break;

                // Shift byte at adress HL right arithmetic
                case 0x2E:
                {
                    uint8_t hl = get_byte(HL);
                    shift_right(hl, false);
                    write_byte(HL, hl);
                    break;
                }

                // Shift 8 bit register bits right logical
                case 0x38: case 0x39: case 0x3A: case 0x3B: case 0x3C: case 0x3D: case 0x3F:
                    shift_right(*reg8_group[opcode - 0x38], true);
                    break;

                // Shift byte at adress HL right logical
                case 0x3E:
                {
                    uint8_t hl = get_byte(HL);
                    shift_right(hl, true);
                    write_byte(HL, hl);
                    break;
                }

                // Test bit of 8 bit register
                case 0x40: case 0x41: case 0x42: case 0x43: case 0x44: case 0x45: case 0x47: // Bit 0
                case 0x48: case 0x49: case 0x4A: case 0x4B: case 0x4C: case 0x4D: case 0x4F: // Bit 1
                case 0x50: case 0x51: case 0x52: case 0x53: case 0x54: case 0x55: case 0x57: // Bit 2
                case 0x58: case 0x59: case 0x5A: case 0x5B: case 0x5C: case 0x5D: case 0x5F: // Bit 3
                case 0x60: case 0x61: case 0x62: case 0x63: case 0x64: case 0x65: case 0x67: // Bit 4
                case 0x68: case 0x69: case 0x6A: case 0x6B: case 0x6C: case 0x6D: case 0x6F: // Bit 5
                case 0x70: case 0x71: case 0x72: case 0x73: case 0x74: case 0x75: case 0x77: // Bit 6
                case 0x78: case 0x79: case 0x7A: case 0x7B: case 0x7C: case 0x7D: case 0x7F: // Bit 7
                    set_flag(7, !get_bit(*reg8_group[opcode & 7], opcode >> 3 & 7));
                    set_flag(6, 0);
                    set_flag(5, 1);
                    break;

                // Test bit of byte at adress HL
                case 0x46: case 0x4E: case 0x56: case 0x5E: case 0x66: case 0x6E: case 0x76: case 0x7E:
                {
                    uint8_t hl = get_byte(HL);
                    set_flag(7, !get_bit(HL, (opcode - 0x46) >> 3));
                    set_flag(6, 0);
                    set_flag(5, 1);
                    break;
                }

                // Set bit of 8 bit register
                case 0xC0: case 0xC1: case 0xC2: case 0xC3: case 0xC4: case 0xC5: case 0xC7: // Bit 0
                case 0xC8: case 0xC9: case 0xCA: case 0xCB: case 0xCC: case 0xCD: case 0xCF: // Bit 1
                case 0xD0: case 0xD1: case 0xD2: case 0xD3: case 0xD4: case 0xD5: case 0xD7: // Bit 2
                case 0xD8: case 0xD9: case 0xDA: case 0xDB: case 0xDC: case 0xDD: case 0xDF: // Bit 3
                case 0xE0: case 0xE1: case 0xE2: case 0xE3: case 0xE4: case 0xE5: case 0xE7: // Bit 4
                case 0xE8: case 0xE9: case 0xEA: case 0xEB: case 0xEC: case 0xED: case 0xEF: // Bit 5
                case 0xF0: case 0xF1: case 0xF2: case 0xF3: case 0xF4: case 0xF5: case 0xF7: // Bit 6
                case 0xF8: case 0xF9: case 0xFA: case 0xFB: case 0xFC: case 0xFD: case 0xFF: // Bit 7
                {
                    uint8_t n = *reg8_group[opcode & 7];
                    set_bit(n, opcode >> 3 & 7, 1);
                    *reg8_group[opcode & 7] = n;
                    break;
                }

                // Set bit of byte at adress HL
                case 0xC6: case 0xCE: case 0xD6: case 0xDE: case 0xE6: case 0xEE: case 0xF6: case 0xFE:
                {
                    uint8_t hl = get_byte(HL);
                    set_bit(hl, (opcode - 0xC6) >> 3, 1);
                    write_byte(HL, hl);
                    break;
                }

                // Reset bit of 8 bit register
                case 0x80: case 0x81: case 0x82: case 0x83: case 0x84: case 0x85: case 0x87: // Bit 0
                case 0x88: case 0x89: case 0x8A: case 0x8B: case 0x8C: case 0x8D: case 0x8F: // Bit 1
                case 0x90: case 0x91: case 0x92: case 0x93: case 0x94: case 0x95: case 0x97: // Bit 2
                case 0x98: case 0x99: case 0x9A: case 0x9B: case 0x9C: case 0x9D: case 0x9F: // Bit 3
                case 0xA0: case 0xA1: case 0xA2: case 0xA3: case 0xA4: case 0xA5: case 0xA7: // Bit 4
                case 0xA8: case 0xA9: case 0xAA: case 0xAB: case 0xAC: case 0xAD: case 0xAF: // Bit 5
                case 0xB0: case 0xB1: case 0xB2: case 0xB3: case 0xB4: case 0xB5: case 0xB7: // Bit 6
                case 0xB8: case 0xB9: case 0xBA: case 0xBB: case 0xBC: case 0xBD: case 0xBF: // Bit 7
                {
                    uint8_t n = *reg8_group[opcode & 7];
                    set_bit(n, opcode >> 3 & 7, 0);
                    *reg8_group[opcode & 7] = n;
                    break;
                }

                // Reset bit of byte at adress HL
                case 0x86: case 0x8E: case 0x96: case 0x9E: case 0xA6: case 0xAE: case 0xB6: case 0xBE:
                {
                    uint8_t hl = get_byte(HL);
                    set_bit(hl, (opcode - 0xC6) >> 3, 0);
                    write_byte(HL, hl);
                    break;
                }
                
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
