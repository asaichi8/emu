#include "CPU.h"

#define i(x, y, z) {&CPU::x, &CPU::y, z}

// http://www.oxyron.de/html/opcodes02.html
CPU::Instruction CPU::instructions[256] = {
/*                 x0              x1              x2              x3              x4              x5              x6              x7                x8              x9              xA              xB              xC              xD              xE              xF              */
/* 0x */    i(BRK, IMP, 7), i(ORA, IZX, 6), i(KIL, IMP, 0), i(SLO, IZX, 8), i(NOP, ZPG, 3), i(ORA, ZPG, 3), i(ASL, ZPG, 5), i(SLO, ZPG, 5),   i(PHP, IMP, 3), i(ORA, IMM, 2), i(ASL, IMP, 2), i(ANC, IMM, 2), i(NOP, ABS, 4), i(ORA, ABS, 4), i(ASL, ABS, 6), i(SLO, ABS, 6), 
/* 1x */    i(BPL, REL, 2), i(ORA, IZY, 5), i(KIL, IMP, 0), i(SLO, IZY, 8), i(NOP, ZPX, 4), i(ORA, ZPX, 4), i(ASL, ZPX, 6), i(SLO, ZPX, 6),   i(CLC, IMP, 2), i(ORA, ABY, 4), i(NOP, IMP, 2), i(SLO, ABY, 7), i(NOP, ABX, 4), i(ORA, ABX, 4), i(ASL, ABX, 7), i(SLO, ABX, 7), 
/* 2x */    i(JSR, ABS, 6), i(AND, IZX, 6), i(KIL, IMP, 0), i(RLA, IZX, 8), i(BIT, ZPG, 3), i(AND, ZPG, 3), i(ROL, ZPG, 5), i(RLA, ZPG, 5),   i(PLP, IMP, 4), i(AND, IMM, 2), i(ROL, IMP, 2), i(ANC, IMM, 2), i(BIT, ABS, 4), i(AND, ABS, 4), i(ROL, ABS, 6), i(RLA, ABS, 6), 
/* 3x */    i(BMI, REL, 2), i(AND, IZY, 5), i(KIL, IMP, 0), i(RLA, IZY, 8), i(NOP, ZPX, 4), i(AND, ZPX, 4), i(ROL, ZPX, 6), i(RLA, ZPX, 6),   i(SEC, IMP, 2), i(AND, ABY, 4), i(NOP, IMP, 2), i(RLA, ABY, 7), i(NOP, ABX, 4), i(AND, ABX, 4), i(ROL, ABX, 7), i(RLA, ABX, 7), 
/* 4x */    i(RTI, IMP, 6), i(EOR, IZX, 6), i(KIL, IMP, 0), i(SRE, IZX, 8), i(NOP, ZPG, 3), i(EOR, ZPG, 3), i(LSR, ZPG, 5), i(SRE, ZPG, 5),   i(PHA, IMP, 3), i(EOR, IMM, 2), i(LSR, IMP, 2), i(ALR, IMM, 2), i(JMP, ABS, 3), i(EOR, ABS, 4), i(LSR, ABS, 6), i(SRE, ABS, 6), 
/* 5x */    i(BVC, REL, 2), i(EOR, IZY, 5), i(KIL, IMP, 0), i(SRE, IZY, 8), i(NOP, ZPX, 4), i(EOR, ZPX, 4), i(LSR, ZPX, 6), i(SRE, ZPX, 6),   i(CLI, IMP, 2), i(EOR, ABY, 4), i(NOP, IMP, 2), i(SRE, ABY, 7), i(NOP, ABX, 4), i(EOR, ABX, 4), i(LSR, ABX, 7), i(SRE, ABX, 7), 
/* 6x */    i(RTS, IMP, 6), i(ADC, IZX, 6), i(KIL, IMP, 0), i(RRA, IZX, 8), i(NOP, ZPG, 3), i(ADC, ZPG, 3), i(ROR, ZPG, 5), i(RRA, ZPG, 5),   i(PLA, IMP, 4), i(ADC, IMM, 2), i(ROR, IMP, 2), i(ARR, IMM, 2), i(JMP, IND, 5), i(ADC, ABS, 4), i(ROR, ABS, 6), i(RRA, ABS, 6), 
/* 7x */    i(BVS, REL, 2), i(ADC, IZY, 5), i(KIL, IMP, 0), i(RRA, IZY, 8), i(NOP, ZPX, 4), i(ADC, ZPX, 4), i(ROR, ZPX, 6), i(RRA, ZPX, 6),   i(SEI, IMP, 2), i(ADC, ABY, 4), i(NOP, IMP, 2), i(RRA, ABY, 7), i(NOP, ABX, 4), i(ADC, ABX, 4), i(ROR, ABX, 7), i(RRA, ABX, 7), 
    
/* 8x */    i(NOP, IMM, 2), i(STA, IZX, 6), i(NOP, IMM, 2), i(SAX, IZX, 6), i(STY, ZPG, 3), i(STA, ZPG, 3), i(STX, ZPG, 3), i(SAX, ZPG, 3),   i(DEY, IMP, 2), i(NOP, IMM, 2), i(TXA, IMP, 2), i(XAA, IMM, 2), i(STY, ABS, 4), i(STA, ABS, 4), i(STX, ABS, 4), i(SAX, ABS, 4), 
/* 9x */    i(BCC, REL, 2), i(STA, IZY, 6), i(KIL, IMP, 0), i(AHX, IZY, 6), i(STY, ZPX, 4), i(STA, ZPX, 4), i(STX, ZPY, 4), i(SAX, ZPY, 4),   i(TYA, IMP, 2), i(STA, ABY, 5), i(TXS, IMP, 2), i(TAS, ABY, 5), i(SHY, ABX, 5), i(STA, ABX, 5), i(SHX, ABY, 5), i(AHX, ABY, 5), 
/* Ax */    i(LDY, IMM, 2), i(LDA, IZX, 6), i(LDX, IMM, 2), i(LAX, IZX, 6), i(LDY, ZPG, 3), i(LDA, ZPG, 3), i(LDX, ZPG, 3), i(LAX, ZPG, 3),   i(TAY, IMP, 2), i(LDA, IMM, 2), i(TAX, IMP, 2), i(LAX, IMM, 2), i(LDY, ABS, 4), i(LDA, ABS, 4), i(LDX, ABS, 4), i(LAX, ABS, 4), 
/* Bx */    i(BCS, REL, 2), i(LDA, IZY, 5), i(KIL, IMP, 0), i(LAX, IZY, 5), i(LDY, ZPX, 4), i(LDA, ZPX, 4), i(LDX, ZPY, 4), i(LAX, ZPY, 4),   i(CLV, IMP, 2), i(LDA, ABY, 4), i(TSX, IMP, 2), i(LAS, ABY, 4), i(LDY, ABX, 4), i(LDA, ABX, 4), i(LDX, ABY, 4), i(LAX, ABY, 4), 
/* Cx */    i(CPY, IMM, 2), i(CMP, IZX, 6), i(NOP, IMM, 2), i(DCP, IZX, 8), i(CPY, ZPG, 3), i(CMP, ZPG, 3), i(DEC, ZPG, 5), i(DCP, ZPG, 5),   i(INY, IMP, 2), i(CMP, IMM, 2), i(DEX, IMP, 2), i(AXS, IMM, 2), i(CPY, ABS, 4), i(CMP, ABS, 4), i(DEC, ABS, 6), i(DCP, ABS, 6), 
/* Dx */    i(BNE, REL, 2), i(CMP, IZY, 5), i(KIL, IMP, 0), i(DCP, IZY, 8), i(NOP, ZPX, 4), i(CMP, ZPX, 4), i(DEC, ZPX, 6), i(DCP, ZPX, 6),   i(CLD, IMP, 2), i(CMP, ABY, 4), i(NOP, IMP, 2), i(DCP, ABY, 7), i(NOP, ABX, 4), i(CMP, ABX, 4), i(DEC, ABX, 7), i(DCP, ABX, 7), 
/* Ex */    i(CPX, IMM, 2), i(SBC, IZX, 6), i(NOP, IMM, 2), i(ISC, IZX, 8), i(CPX, ZPG, 3), i(SBC, ZPG, 3), i(INC, ZPG, 5), i(ISC, ZPG, 5),   i(INX, IMP, 2), i(SBC, IMM, 2), i(NOP, IMP, 2), i(SBC, IMM, 2), i(CPX, ABS, 4), i(SBC, ABS, 4), i(INC, ABS, 6), i(ISC, ABS, 6), 
/* Fx */    i(BEQ, REL, 2), i(SBC, IZY, 5), i(KIL, IMP, 0), i(ISC, IZY, 8), i(NOP, ZPX, 4), i(SBC, ZPX, 4), i(INC, ZPX, 6), i(ISC, ZPX, 6),   i(SED, IMP, 2), i(SBC, ABY, 4), i(NOP, IMP, 2), i(ISC, ABY, 7), i(NOP, ABX, 4), i(SBC, ABX, 4), i(INC, ABX, 7), i(ISC, ABX, 7)

};

// TODO: package status register flag setters into functions

// Load operations
void CPU::LDA(WORD addr) 
{ 
    reg.accumulator = m_RAM->ReadByte(addr);
    
    reg.CheckNegative(reg.accumulator);
    reg.CheckZero(reg.accumulator);
}

void CPU::LDX(WORD addr) 
{ 
    reg.X = m_RAM->ReadByte(addr);
    
    reg.CheckNegative(reg.X);
    reg.CheckZero(reg.X);
}

void CPU::LDY(WORD addr) 
{ 
    reg.Y = m_RAM->ReadByte(addr);
    
    reg.CheckNegative(reg.Y);
    reg.CheckZero(reg.Y);
}


// Store operations
void CPU::STA(WORD addr) 
{ 
    m_RAM->WriteByte(addr, reg.accumulator);
}

void CPU::STX(WORD addr) 
{ 
    m_RAM->WriteByte(addr, reg.X);
}

void CPU::STY(WORD addr) 
{ 
    m_RAM->WriteByte(addr, reg.Y);
}


// Increment operations
void CPU::INC(WORD addr) 
{ 
    BYTE val = m_RAM->ReadByte(addr);
    val++;

    reg.CheckNegative(val);
    reg.CheckZero(val);

    m_RAM->WriteByte(addr, val);
}

void CPU::INX(WORD addr) 
{ 
    reg.X++;

    reg.CheckNegative(reg.X);
    reg.CheckZero(reg.X);
}

void CPU::INY(WORD addr) 
{ 
    reg.Y++;

    reg.CheckNegative(reg.Y);
    reg.CheckZero(reg.Y);
}


// Decrement operations
void CPU::DEC(WORD addr) 
{ 
    BYTE val = m_RAM->ReadByte(addr);
    val--;

    reg.CheckNegative(val);
    reg.CheckZero(val);
        
    m_RAM->WriteByte(addr, val);
}

void CPU::DEX(WORD addr) 
{ 
    reg.X--;

    reg.CheckNegative(reg.X);
    reg.CheckZero(reg.X);
}

void CPU::DEY(WORD addr) 
{ 
    reg.Y--;

    reg.CheckNegative(reg.Y);
    reg.CheckZero(reg.Y);
}


// Arithmetic operations
void CPU::ADC(WORD addr) 
{ 
    BYTE val = m_RAM->ReadByte(addr);

    WORD added16 = val + reg.accumulator; // add values together
    
    // add carry bit if present
    if (reg.status_register.test(StatusRegisterFlags::CARRY))
        added16++;
    
    BYTE result = added16 & 0x00FF; // convert to 8-bit
    
    //  registers
    reg.CheckCarry(added16);
    reg.CheckZero(result);
    reg.CheckNegative(result);

    // determine overflow bit
    // if positive + positive = negative, overflow occured
    if ((CHAR)reg.accumulator >= 0 && (CHAR)val >= 0 && (CHAR)(result) < 0)
        reg.status_register.set(StatusRegisterFlags::OVERFLOW);
    // if negative + negative = positive, overflow occured
    else if ((CHAR)reg.accumulator < 0 && (CHAR)val < 0 && (CHAR)(result) >= 0)
        reg.status_register.set(StatusRegisterFlags::OVERFLOW);
    else
        reg.status_register.reset(StatusRegisterFlags::OVERFLOW);
    

    reg.accumulator = result; // save result to accumulator
}

void CPU::SBC(WORD addr) 
{ 
    BYTE val = m_RAM->ReadByte(addr);

    WORD negated16 = reg.accumulator - val; // negate values

    // subtract only if carry bit is not set
    if (!reg.status_register.test(StatusRegisterFlags::CARRY))
        negated16--;

    BYTE result = negated16 & 0x00FF; // convert to 8-bit

    //  registers
    reg.CheckCarry(negated16);
    reg.CheckZero(result);
    reg.CheckNegative(result);

    // determine overflow bit
    // if negative - positive = positive, overflow occured
    if ((CHAR)reg.accumulator < 0 && (CHAR)val >= 0 && (CHAR)(result) >= 0)
        reg.status_register.set(StatusRegisterFlags::OVERFLOW);
    // if positive - negative = negative, overflow occured
    else if ((CHAR)reg.accumulator >= 0 && (CHAR)val < 0 && (CHAR)(result) < 0)
        reg.status_register.set(StatusRegisterFlags::OVERFLOW);
    else
        reg.status_register.reset(StatusRegisterFlags::OVERFLOW);

    reg.accumulator = result; // save result to accumulator
}

void CPU::CMP(WORD addr) 
{ 
    BYTE val = m_RAM->ReadByte(addr);
    WORD negated16 = reg.accumulator - val;

    BYTE result = negated16 & 0x00FF;

    reg.CheckCarryCompare(reg.accumulator, val);
    reg.CheckZero(result);
    reg.CheckNegative(result);
}

void CPU::CPX(WORD addr) 
{ 
    BYTE val = m_RAM->ReadByte(addr);
    WORD negated16 = reg.X - val;

    BYTE result = negated16 & 0x00FF;

    reg.CheckCarryCompare(reg.X, val);
    reg.CheckZero(result);
    reg.CheckNegative(result);
}

void CPU::CPY(WORD addr) 
{ 
    BYTE val = m_RAM->ReadByte(addr);
    WORD negated16 = reg.Y - val;

    BYTE result = negated16 & 0x00FF;

    reg.CheckCarryCompare(reg.Y, val);
    reg.CheckZero(result);
    reg.CheckNegative(result);
}


// Logical operations
void CPU::AND(WORD addr) 
{ 
    BYTE val = m_RAM->ReadByte(addr);

    BYTE result = val & reg.accumulator;

    reg.CheckZero(result);
    reg.CheckNegative(result);

    reg.accumulator = result;
}

void CPU::EOR(WORD addr) 
{ 
    BYTE val = m_RAM->ReadByte(addr);

    BYTE result = val ^ reg.accumulator;

    reg.CheckZero(result);
    reg.CheckNegative(result);

    reg.accumulator = result;
}

void CPU::ORA(WORD addr) 
{ 
    BYTE val = m_RAM->ReadByte(addr);

    BYTE result = val | reg.accumulator;

    reg.CheckZero(result);
    reg.CheckNegative(result);

    reg.accumulator = result;
}

void CPU::BIT(WORD addr) 
{ 
    BYTE val = m_RAM->ReadByte(addr);

    BYTE result = reg.accumulator & val;

    // http://www.6502.org/users/obelisk/6502/reference.html#BIT
    reg.CheckZero(result);
    reg.CheckNegative(val);
    // set overflow bit based on 6th bit of memory value
    reg.status_register.set(StatusRegisterFlags::OVERFLOW, val & (1 << 6));
}


// Stack operations
void CPU::PHA(WORD addr) 
{ 
    PushStackByte(reg.accumulator);
}

void CPU::PHP(WORD addr) 
{ 
    PushStackByte((BYTE)reg.status_register.to_ulong());
}

void CPU::PLA(WORD addr) 
{ 
    reg.accumulator = PopStackByte();

    reg.CheckZero(reg.accumulator);
    reg.CheckNegative(reg.accumulator);
}

void CPU::PLP(WORD addr) 
{ 
    reg.status_register = PopStackByte();
}


// Jump operations
void CPU::JMP(WORD addr) 
{ 
    reg.program_counter = addr;
}

// TODO: check if this is correct
void CPU::JSR(WORD addr) 
{ 
    reg.program_counter--;

    PushStackWord(reg.program_counter);

    reg.program_counter = addr;
}

void CPU::RTS(WORD addr) 
{ 
    reg.program_counter = PopStackWord();

    reg.program_counter++;
}


// Branch operations
void CPU::BCC(WORD addr) 
{ 
    if (!reg.status_register.test(StatusRegisterFlags::CARRY))
        reg.program_counter = addr;
}

void CPU::BCS(WORD addr) 
{ 
    if (reg.status_register.test(StatusRegisterFlags::CARRY))
        reg.program_counter = addr;
}

void CPU::BEQ(WORD addr) 
{ 
    if (reg.status_register.test(StatusRegisterFlags::ZERO))
        reg.program_counter = addr;
}

void CPU::BMI(WORD addr) 
{ 
    if (reg.status_register.test(StatusRegisterFlags::NEGATIVE))
        reg.program_counter = addr;
}

void CPU::BNE(WORD addr) 
{ 
    if (!reg.status_register.test(StatusRegisterFlags::ZERO))
        reg.program_counter = addr;
}

void CPU::BPL(WORD addr) 
{ 
    if (!reg.status_register.test(StatusRegisterFlags::NEGATIVE))
        reg.program_counter = addr;
}

void CPU::BVC(WORD addr) 
{ 
    if (!reg.status_register.test(StatusRegisterFlags::OVERFLOW))
        reg.program_counter = addr;
}

void CPU::BVS(WORD addr) 
{ 
    if (reg.status_register.test(StatusRegisterFlags::OVERFLOW))
        reg.program_counter = addr;
}


// Shift operations
void CPU::ASL(WORD addr) 
{ 
    BYTE val{};
    bool isImplicit = (instructions[m_curOpcode].addrMode == &CPU::IMP);
    
    if (isImplicit)
        val = m_RAM->ReadByte(addr);
    else
        val = reg.accumulator;


    // set carry flag if 7th bit (the one we're about to shift away) is set
    reg.status_register.set(StatusRegisterFlags::CARRY, val & 0x80);

    BYTE result = val << 1;
    reg.CheckNegative(result);
    reg.CheckZero(result);
    

    if (isImplicit)
        m_RAM->WriteByte(addr, result); // write value back to same location
    else
        reg.accumulator = result;
}

void CPU::LSR(WORD addr) 
{ 
    BYTE val{};
    bool isImplicit = (instructions[m_curOpcode].addrMode == &CPU::IMP);
    
    if (isImplicit)
        val = m_RAM->ReadByte(addr);
    else
        val = reg.accumulator;


    // set carry flag if 1st bit (the one we're about to shift away) is set
    reg.status_register.set(StatusRegisterFlags::CARRY, val & 0x01);

    BYTE result = val >> 1;
    reg.CheckNegative(result);
    reg.CheckZero(result);
    

    if (isImplicit)
        m_RAM->WriteByte(addr, result); // write value back to same location
    else
        reg.accumulator = result;
}

void CPU::ROL(WORD addr) 
{ 
    BYTE val{};
    bool isImplicit = (instructions[m_curOpcode].addrMode == &CPU::IMP);
    
    if (isImplicit)
        val = m_RAM->ReadByte(addr);
    else
        val = reg.accumulator;


    // set carry flag if 7th bit (the one we're about to shift away) is set
    reg.status_register.set(StatusRegisterFlags::CARRY, val & 0x80);

    // shift to left while setting bit 0 to current value of carry flag
    BYTE result = (val << 1);
    result += reg.status_register.test(StatusRegisterFlags::CARRY);

    reg.CheckNegative(result);
    reg.CheckZero(result);
    

    if (isImplicit)
        m_RAM->WriteByte(addr, result); // write value back to same location
    else
        reg.accumulator = result;
}

void CPU::ROR(WORD addr) 
{ 
    BYTE val{};
    bool isImplicit = (instructions[m_curOpcode].addrMode == &CPU::IMP);
    
    if (isImplicit)
        val = m_RAM->ReadByte(addr);
    else
        val = reg.accumulator;


    // set carry flag if 1st bit (the one we're about to shift away) is set
    reg.status_register.set(StatusRegisterFlags::CARRY, val & 0x01);

    // shift to right while setting bit 7 to current value of carry flag
    BYTE result = (val >> 1);
    result += reg.status_register.test(StatusRegisterFlags::CARRY) << 7;

    reg.CheckNegative(result);
    reg.CheckZero(result);
    

    if (isImplicit)
        m_RAM->WriteByte(addr, result); // write value back to same location
    else
        reg.accumulator = result;
}


// Clear operations
void CPU::CLC(WORD addr) 
{ 
    reg.status_register.reset(StatusRegisterFlags::CARRY);
}

void CPU::CLD(WORD addr) 
{ 
    reg.status_register.reset(StatusRegisterFlags::DECIMAL_MODE);
}

void CPU::CLI(WORD addr) 
{ 
    reg.status_register.reset(StatusRegisterFlags::INTERRUPT_REQUEST);
}

void CPU::CLV(WORD addr) 
{ 
    reg.status_register.reset(StatusRegisterFlags::OVERFLOW);
}


// Set flag operations
void CPU::SEC(WORD addr) 
{ 
    reg.status_register.set(StatusRegisterFlags::CARRY);
}

void CPU::SED(WORD addr) 
{ 
    reg.status_register.set(StatusRegisterFlags::DECIMAL_MODE);
}

void CPU::SEI(WORD addr) 
{ 
    reg.status_register.set(StatusRegisterFlags::INTERRUPT_REQUEST);
}


// Transfer operations
void CPU::TAX(WORD addr) 
{ 
    reg.X = reg.accumulator;

    reg.CheckNegative(reg.X);
    reg.CheckZero(reg.X);
}

void CPU::TAY(WORD addr) 
{ 
    reg.Y = reg.accumulator;

    reg.CheckNegative(reg.Y);
    reg.CheckZero(reg.Y);
}

void CPU::TXA(WORD addr) 
{ 
    reg.accumulator = reg.X;

    reg.CheckNegative(reg.accumulator);
    reg.CheckZero(reg.accumulator);
}

void CPU::TYA(WORD addr) 
{ 
    reg.accumulator = reg.Y;

    reg.CheckNegative(reg.Y);
    reg.CheckZero(reg.Y);
}

void CPU::TSX(WORD addr) 
{ 
    reg.X = reg.stack_pointer;

    reg.CheckZero(reg.X);
    reg.CheckNegative(reg.X);
}

void CPU::TXS(WORD addr) 
{ 
    reg.stack_pointer = reg.X;
}


// System functions
void CPU::BRK(WORD addr) 
{ 
    reg.program_counter++;

    PushStackWord(reg.program_counter);
    PushStackByte((BYTE)(reg.status_register.to_ulong()));

    reg.status_register.set(StatusRegisterFlags::INTERRUPT_REQUEST);

    reg.program_counter = m_RAM->ReadByte(IRQ_VECTOR);
}

void CPU::NOP(WORD addr) 
{ 
    //
}

void CPU::RTI(WORD addr) 
{ 
    reg.status_register = PopStackByte();
    reg.program_counter = PopStackWord();
}


// Illegal/unused operations
void CPU::SHY(WORD addr) 
{ 

}

void CPU::TAS(WORD addr) 
{ 

}

void CPU::SHX(WORD addr) 
{ 

}

void CPU::AHX(WORD addr) 
{ 

}

void CPU::XAA(WORD addr) 
{ 

}

void CPU::LAX(WORD addr) 
{ 

}

void CPU::KIL(WORD addr) 
{ 

}

void CPU::SLO(WORD addr) 
{ 

}

void CPU::RLA(WORD addr) 
{ 

}

void CPU::SRE(WORD addr) 
{ 

}

void CPU::RRA(WORD addr) 
{ 

}

void CPU::SAX(WORD addr) 
{ 

}

void CPU::DCP(WORD addr) 
{ 

}

void CPU::ANC(WORD addr) 
{ 

}

void CPU::ALR(WORD addr) 
{ 

}

void CPU::ARR(WORD addr) 
{ 

}

void CPU::LAS(WORD addr) 
{ 

}

void CPU::AXS(WORD addr) 
{ 

}

void CPU::ISC(WORD addr) 
{ 

}