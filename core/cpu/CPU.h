#pragma once

#include <bitset>
#include "RAM.h"
#include "../../include/typedefs.h"

#define MODE(addressingMode) WORD addressingMode()
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
    MODE(IMM); // immediate
    MODE(IMP); // implicit
    MODE(ACC); // accumulator
    MODE(ABS); // absolute
    MODE(ABX); // absolute x
    MODE(ABY); // absolute y
    MODE(IND); // indirect
    MODE(REL); // relative
    MODE(ZP); // zero page
    MODE(ZPX); // zero page x
    MODE(ZPY); // zero page y
    MODE(IZY); // indirect indexed
    MODE(IZX); // indexed indirect

    // --== Opcodes ==--
    // http://www.oxyron.de/html/opcodes02.html
    // https://www.nesdev.org/wiki/CPU_unofficial_opcodes
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
    OPCODE(ADC);
    OPCODE(SBC);
    OPCODE(CMP);
    OPCODE(CPX);
    OPCODE(CPY);

    // Logical operations
    OPCODE(AND);
    OPCODE(EOR);
    OPCODE(ORA);
    OPCODE(BIT);

    // Stack operations
    OPCODE(TSX);
    OPCODE(TXS);
    OPCODE(PHA);
    OPCODE(PHP);
    OPCODE(PLA);
    OPCODE(PLP);

    // Jump operations
    OPCODE(JMP);
    OPCODE(JSR);
    OPCODE(RTS);

    // Branch operations
    OPCODE(BCC);
    OPCODE(BCS);
    OPCODE(BEQ);
    OPCODE(BMI);
    OPCODE(BNE);
    OPCODE(BPL);
    OPCODE(BVC);
    OPCODE(BVS);

    // Shift operations
    OPCODE(ASL);
    OPCODE(LSR);
    OPCODE(ROL);
    OPCODE(ROR);
    
    // Clear operations
    OPCODE(CLC);
    OPCODE(CLD);
    OPCODE(CLI);
    OPCODE(CLV);

    // Set flag operations
    OPCODE(SEC);
    OPCODE(SED);
    OPCODE(SEI);

    // Transfer operations
    OPCODE(TAX);
    OPCODE(TAY);
    OPCODE(TXA);
    OPCODE(TYA);

    // System functions
    OPCODE(BRK);
    OPCODE(NOP);
    OPCODE(RTI);


    // Illegal/unused operations
    // https://www.masswerk.at/nowgobang/2021/6502-illegal-opcodes



    static Instruction instructions[256];

    void Execute(const Instruction& instruction);
    void Reset();

public:
    CPU(RAM* ram);
};
