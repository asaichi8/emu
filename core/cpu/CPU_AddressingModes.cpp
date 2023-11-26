#include "CPU.h"


// TODO: probably a bunch of wraparound/overflow bugs
//       usage of ReadWord is questionable

// After fetching the opcode, the program counter points to the effective operand.

/// @brief Takes a single constant byte as an operand, e.g. LDY #$05, loading 0x05 into the Y index register. 
WORD CPU::IMM()
{
    return reg.program_counter++; // return addr of operand, then increment pc
}

/// @brief Does not take any operand – the instruction is self-explanatory (e.g. NOP – no operation is done.) 
WORD CPU::IMP()
{
    return 0; // not needed
}

/// @brief Takes a single operand, ‘A’. Operates directly on the accumulator register
///        (e.g. ROL A, rotating the contents of the accumulator left one bit.)
WORD CPU::ACC()
{
    return 0; // not needed
}

/// @brief Takes a two-byte address as an operand (e.g. JMP $3000.)
WORD CPU::ABS()
{
    WORD addr = m_RAM->ReadWord(reg.program_counter);
    reg.program_counter += 2;

    return addr;
}

/// @brief Like the absolute addressing mode, but with an extra operand containing one of the 8-bit index registers.
///        The contents of said register will be added to the address of the first two-byte operand,
///        evaluating to a final 16-bit address (e.g. LDY $1234, X would load the byte contained at address $1234 + X
///        into the Y register. Therefore, if X contained the value 10H, the byte contained at memory location $1244
///        would be loaded into the Y register.)
WORD CPU::ABX()
{
    return ABS() + reg.X;
}
WORD CPU::ABY()
{
    return ABS() + reg.Y;
}

/// @brief Takes a two-byte address which should resolve to another two-byte address as an operand. For example, if
///        the operand is $5000 (e.g. JMP ($5000)) and the values of $5000 and $5001 are $EF and $BE respectively,
///        the instruction will execute with these values in absolute addressing mode fashion (using the previous
///        example, JMP ($5000) would evaluate to JMP ($BEEF)).
WORD CPU::IND()
{
    WORD addr = ABS();
    
    return m_RAM->ReadWord(addr);
}

/// @brief Takes a single operand containing a signed 8-bit number (ranging from -128 to +127) as a label.
WORD CPU::REL()
{
    WORD addr = m_RAM->ReadByte(reg.program_counter); // read the offset
    reg.program_counter++;

    int8_t offset = (int8_t)addr;
    addr = reg.program_counter + (WORD)offset; // increment/decrement pc by the offset

    return addr;
}

/// @brief Instructions utilising this mode only take one 8-bit operand denoting a location inside the zero page
///        (e.g. DEC $5F would decrement the byte found at $005F by one.)
WORD CPU::ZPG()
{
    return m_RAM->ReadByte(reg.program_counter++);
}

WORD CPU::ZPX()
{
    return (ZPG() + reg.X) & 0x00FF;
}

WORD CPU::ZPY()
{
    return (ZPG() + reg.Y) & 0x00FF;
}

WORD CPU::IZY()
{
    /* e.g.: MEMORY ADDR | CONTENTS |                      DESCRIPTION
                0x1000   |   0x60   | Supplied zero page address (inside reg.program_counter)
                0x0060   |   0x00   | Low byte of effective address in the zero page
                0x0061   |   0x70   | High byte of effective address in the zero page

        Y = 0x40
    */

    WORD zpAddrLow = m_RAM->ReadByte(reg.program_counter); // Read supplied ZP address, result: 0x60 (low byte of effective address)
    reg.program_counter++; // Increment PC: it's now 0x1001

    return m_RAM->ReadWord(zpAddrLow, true) + reg.Y; // Read word at 0x60-0x61 (0x7000). Add Y for a final result of 0x7040.
}

WORD CPU::IZX()
{
    WORD zpAddrLow = m_RAM->ReadByte(reg.program_counter);
    reg.program_counter++;

    WORD addr = (zpAddrLow + reg.X) & 0x00FF; // add X and wrap around
    return m_RAM->ReadWord(addr, true);
}

WORD CPU::unk()
{
    return 0;
}