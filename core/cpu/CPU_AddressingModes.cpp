#include "CPU.h"


// TODO: probably a bunch of wraparound/overflow bugs
//       usage of ReadWord is questionable

// After fetching the opcode, the program counter points to the effective operand.

/// @brief Takes a single constant byte as an operand, e.g. LDY #$05, loading 0x05 into the Y index register. 
WORD CPU::mode_immediate()
{
    return reg.program_counter++; // return addr of operand, then increment pc
}

/// @brief Does not take any operand – the instruction is self-explanatory (e.g. NOP – no operation is done.) 
WORD CPU::mode_implicit()
{
    return NULL; // not needed
}

/// @brief Takes a single operand, ‘A’. Operates directly on the accumulator register
///        (e.g. ROL A, rotating the contents of the accumulator left one bit.)
WORD CPU::mode_accumulator()
{
    return NULL; // not needed
}

/// @brief Takes a two-byte address as an operand (e.g. JMP $3000.)
WORD CPU::mode_absolute()
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
WORD CPU::mode_absolute_x()
{
    return mode_absolute() + reg.X;
}
WORD CPU::mode_absolute_y()
{
    return mode_absolute() + reg.Y;
}

/// @brief Takes a two-byte address which should resolve to another two-byte address as an operand. For example, if
///        the operand is $5000 (e.g. JMP ($5000)) and the values of $5000 and $5001 are $EF and $BE respectively,
///        the instruction will execute with these values in absolute addressing mode fashion (using the previous
///        example, JMP ($5000) would evaluate to JMP ($BEEF)).
WORD CPU::mode_indirect()
{
    WORD addr = mode_absolute();
    
    return m_RAM->ReadWord(addr);
}

/// @brief Takes a single operand containing a signed 8-bit number (ranging from -128 to +127) as a label.
WORD CPU::mode_relative()
{
    WORD addr = m_RAM->ReadByte(reg.program_counter); // read the offset
    reg.program_counter++;

    int8_t offset = (int8_t)addr;
    addr = reg.program_counter + (WORD)offset; // increment/decrement pc by the offset

    return addr;
}

/// @brief Instructions utilising this mode only take one 8-bit operand denoting a location inside the zero page
///        (e.g. DEC $5F would decrement the byte found at $005F by one.)
WORD CPU::mode_zp()
{
    return m_RAM->ReadByte(reg.program_counter++);
}

WORD CPU::mode_zp_x()
{
    return (mode_zp() + reg.X) & 0x00FF;
}

WORD CPU::mode_zp_y()
{
    return (mode_zp() + reg.Y) & 0x00FF;
}

WORD CPU::mode_indirect_indexed()
{
    WORD addr = m_RAM->ReadWord(reg.program_counter);
    reg.program_counter++;

    return m_RAM->ReadWord(addr) + reg.Y;
}

WORD CPU::mode_indexed_indirect()
{
    WORD addr = m_RAM->ReadWord(reg.program_counter) + reg.X;
    reg.program_counter++;

    return m_RAM->ReadWord(addr);
}