#pragma once

#define SIMULATE_BUGS 1

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
    BYTE m_curOpcode{};

    class registers
    {
        public:
            WORD program_counter{};
            BYTE accumulator{};
            BYTE X{};   
            BYTE Y{};
            BYTE stack_pointer{};
            std::bitset<8> status_register{};

            // Checks if the value is zero, and sets register based on result
            void CheckZero(BYTE val)
            {
                if (val == 0x00)
                    this->status_register.set(StatusRegisterFlags::ZERO);
                else
                    this->status_register.reset(StatusRegisterFlags::ZERO);
            }

            // Checks if the value is negative, and sets register based on result
            void CheckNegative(BYTE val)
            {
                if ((CHAR)val < 0)
                    this->status_register.set(StatusRegisterFlags::NEGATIVE);
                else
                    this->status_register.reset(StatusRegisterFlags::NEGATIVE);
            }

            // Checks if the value is has a carry, and sets register based on result
            void CheckCarry(WORD val)
            {
                // set carry flag if we don't need to borrow, reset otherwise
                if (val <= BYTE_MAX)
                    this->status_register.set(StatusRegisterFlags::CARRY);
                else
                    this->status_register.reset(StatusRegisterFlags::CARRY);
            }
    } reg;

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

public:
    CPU(RAM* ram);

    void Reset();
    void Run();
};
