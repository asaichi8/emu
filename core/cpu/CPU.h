#pragma once

#define SIMULATE_BUGS 1

#include <bitset>
#include <functional>
#include "CPURegisters.h"
#include "RAM.h"
#include "typedefs.h"

#define MODE(addressingMode) WORD addressingMode()
#define OPCODE(opcodeName) void opcodeName(WORD addr)

class CPU
{
    Bus* m_Bus{};
    CPURegisters reg{};
    BYTE m_curOpcode{};
    DWORD m_curCycles{};
    QWORD m_nCycles{};


    struct Instruction
    {
        void (CPU::*opcode)(WORD);
        WORD (CPU::*addrMode)();
        DWORD cycles;
    };

    void PushStackByte(BYTE val);
    BYTE PopStackByte();
    void PushStackWord(WORD val);
    WORD PopStackWord();
    static bool IsOnSamePage(WORD addr1, WORD addr2);


    // Addressing modes - returns the address to act upon
    MODE(IMM); // immediate
    MODE(IMP); // implicit
    MODE(ACC); // accumulator
    MODE(ABS); // absolute
    MODE(ABX); // absolute x
    MODE(ABY); // absolute y
    MODE(IND); // indirect
    MODE(REL); // relative
    MODE(ZPG); // zero page
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
    OPCODE(TSX);
    OPCODE(TXS);

    // System functions
    OPCODE(BRK);
    OPCODE(NOP);
    OPCODE(RTI);


    // Illegal/unused operations
    // https://www.masswerk.at/nowgobang/2021/6502-illegal-opcodes
    OPCODE(SHY);
    OPCODE(TAS);
    OPCODE(SHX);
    OPCODE(AHX);
    OPCODE(XAA);
    OPCODE(LAX);
    OPCODE(KIL);
    OPCODE(SLO);
    OPCODE(RLA);
    OPCODE(SRE);
    OPCODE(RRA);
    OPCODE(SAX);
    OPCODE(DCP);
    OPCODE(ANC);
    OPCODE(ALR);
    OPCODE(ARR);
    OPCODE(LAS);
    OPCODE(AXS);
    OPCODE(ISC);
    

    static Instruction instructions[256];

    void Execute(const Instruction& instruction);
    size_t GetInstructionLenBytes(const Instruction& instruction);

public:
    CPU(Bus* bus_ptr);

    // https://www.pagetable.com/?p=410
    static const WORD NMI_VECTOR = 0xFFFA;
    // https://en.wikipedia.org/wiki/Reset_vector "The reset vector for 6502 processor family is a 16-bit address stored at 0xFFFC and 0xFFFD."
    static const WORD RESET_VECTOR = 0xFFFC;
    // http://www.6502.org/users/obelisk/6502/reference.html#BRK : "IRQ interrupt vector at $FFFE/F"   
    static const WORD IRQ_VECTOR = 0xFFFE;

    void Run();
    void Reset();
    void IRQ();
    void NMI();
    const CPURegisters& ReadRegisters() const { return reg; }
};
