#pragma once

#include <bitset>
#include "RAM.h"
#include "../../include/typedefs.h"

#define OPCODE(opcodeName) void opcodeName(WORD addr)

enum StatusRegisterFlags
{
    CARRY,
    ZERO,
    INTERRUPT_REQUEST,
    DECIMAL_MODE,
    BREAK_COMMAND,
    UNUSED,
    OVERFLOW,
    NEGATIVE
};

class CPU
{
    RAM* m_RAM = nullptr;

    struct registers
    {
        WORD program_counter{};
        BYTE accumulator{};
        BYTE X{};   
        BYTE Y{};
        BYTE stack_pointer{};
        std::bitset<8> status_register{};
    } reg;

    struct Instruction
    {
        void (CPU::*opcode)(WORD);
        WORD (CPU::*addrMode)();
        DWORD cycles;
    };

    // Addressing modes - returns the address to act upon
    WORD mode_immediate();
    WORD mode_implicit();
    WORD mode_accumulator();
    WORD mode_absolute();
    WORD mode_absolute_x();
    WORD mode_absolute_y();
    WORD mode_indirect();
    WORD mode_relative();
    WORD mode_zp();
    WORD mode_zp_x();
    WORD mode_zp_y();
    WORD mode_indirect_indexed();
    WORD mode_indexed_indirect();

    // --== Opcodes ==--
    // Load operations
    OPCODE(LDA);
    OPCODE(LDX);
    OPCODE(LDY);

    // Store operations
    OPCODE(STA);
    OPCODE(STX);
    OPCODE(STY);

    // Increment operations
    OPCODE(INC);
    OPCODE(INX);
    OPCODE(INY);

    // Decrement operations
    OPCODE(DEC);
    OPCODE(DEX);
    OPCODE(DEY);

    // Arithmetic operations

    // Logical operations

    // Stack operations

    // Jump operations

    // Branch operations

    // Shift operations

    // Clear operations

    // Set flag operations

    // System functions

    // Illegal/unused operations




    static Instruction instructions[256];

    void Execute(const Instruction& instruction);
    void Reset();

public:
    CPU(RAM* ram);
};
