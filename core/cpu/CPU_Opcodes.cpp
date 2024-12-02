#include "CPU.h"


#define i(x, y, z, w) {&CPU::x, &CPU::y, z, w}

// http://www.oxyron.de/html/opcodes02.html
CPU::Instruction CPU::instructions[256] = {
/*                 x0                x1                x2                x3                x4                x5                x6                x7                  x8                x9                xA                xB                xC                xD                xE                xF              */
/* 0x */    i(BRK, IMP, 7,0), i(ORA, IZX, 6,0), i(KIL, IMP, 0,0), i(SLO, IZX, 8,0), i(NOP, ZPG, 3,0), i(ORA, ZPG, 3,0), i(ASL, ZPG, 5,0), i(SLO, ZPG, 5,0),   i(PHP, IMP, 3,0), i(ORA, IMM, 2,0), i(ASL, IMP, 2,0), i(ANC, IMM, 2,0), i(NOP, ABS, 4,0), i(ORA, ABS, 4,0), i(ASL, ABS, 6,0), i(SLO, ABS, 6,0), 
/* 1x */    i(BPL, REL, 2,0), i(ORA, IZY, 5,1), i(KIL, IMP, 0,0), i(SLO, IZY, 8,0), i(NOP, ZPX, 4,0), i(ORA, ZPX, 4,0), i(ASL, ZPX, 6,0), i(SLO, ZPX, 6,0),   i(CLC, IMP, 2,0), i(ORA, ABY, 4,1), i(NOP, IMP, 2,0), i(SLO, ABY, 7,0), i(NOP, ABX, 4,1), i(ORA, ABX, 4,1), i(ASL, ABX, 7,0), i(SLO, ABX, 7,0), 
/* 2x */    i(JSR, ABS, 6,0), i(AND, IZX, 6,0), i(KIL, IMP, 0,0), i(RLA, IZX, 8,0), i(BIT, ZPG, 3,0), i(AND, ZPG, 3,0), i(ROL, ZPG, 5,0), i(RLA, ZPG, 5,0),   i(PLP, IMP, 4,0), i(AND, IMM, 2,0), i(ROL, IMP, 2,0), i(ANC, IMM, 2,0), i(BIT, ABS, 4,0), i(AND, ABS, 4,0), i(ROL, ABS, 6,0), i(RLA, ABS, 6,0), 
/* 3x */    i(BMI, REL, 2,0), i(AND, IZY, 5,1), i(KIL, IMP, 0,0), i(RLA, IZY, 8,0), i(NOP, ZPX, 4,0), i(AND, ZPX, 4,0), i(ROL, ZPX, 6,0), i(RLA, ZPX, 6,0),   i(SEC, IMP, 2,0), i(AND, ABY, 4,1), i(NOP, IMP, 2,0), i(RLA, ABY, 7,0), i(NOP, ABX, 4,1), i(AND, ABX, 4,1), i(ROL, ABX, 7,0), i(RLA, ABX, 7,0), 
/* 4x */    i(RTI, IMP, 6,0), i(EOR, IZX, 6,0), i(KIL, IMP, 0,0), i(SRE, IZX, 8,0), i(NOP, ZPG, 3,0), i(EOR, ZPG, 3,0), i(LSR, ZPG, 5,0), i(SRE, ZPG, 5,0),   i(PHA, IMP, 3,0), i(EOR, IMM, 2,0), i(LSR, IMP, 2,0), i(ALR, IMM, 2,0), i(JMP, ABS, 3,0), i(EOR, ABS, 4,0), i(LSR, ABS, 6,0), i(SRE, ABS, 6,0), 
/* 5x */    i(BVC, REL, 2,0), i(EOR, IZY, 5,1), i(KIL, IMP, 0,0), i(SRE, IZY, 8,0), i(NOP, ZPX, 4,0), i(EOR, ZPX, 4,0), i(LSR, ZPX, 6,0), i(SRE, ZPX, 6,0),   i(CLI, IMP, 2,0), i(EOR, ABY, 4,1), i(NOP, IMP, 2,0), i(SRE, ABY, 7,0), i(NOP, ABX, 4,1), i(EOR, ABX, 4,1), i(LSR, ABX, 7,0), i(SRE, ABX, 7,0), 
/* 6x */    i(RTS, IMP, 6,0), i(ADC, IZX, 6,0), i(KIL, IMP, 0,0), i(RRA, IZX, 8,0), i(NOP, ZPG, 3,0), i(ADC, ZPG, 3,0), i(ROR, ZPG, 5,0), i(RRA, ZPG, 5,0),   i(PLA, IMP, 4,0), i(ADC, IMM, 2,0), i(ROR, IMP, 2,0), i(ARR, IMM, 2,0), i(JMP, IND, 5,0), i(ADC, ABS, 4,0), i(ROR, ABS, 6,0), i(RRA, ABS, 6,0), 
/* 7x */    i(BVS, REL, 2,0), i(ADC, IZY, 5,1), i(KIL, IMP, 0,0), i(RRA, IZY, 8,0), i(NOP, ZPX, 4,0), i(ADC, ZPX, 4,0), i(ROR, ZPX, 6,0), i(RRA, ZPX, 6,0),   i(SEI, IMP, 2,0), i(ADC, ABY, 4,1), i(NOP, IMP, 2,0), i(RRA, ABY, 7,0), i(NOP, ABX, 4,1), i(ADC, ABX, 4,1), i(ROR, ABX, 7,0), i(RRA, ABX, 7,0), 
	
/* 8x */    i(NOP, IMM, 2,0), i(STA, IZX, 6,0), i(NOP, IMM, 2,0), i(SAX, IZX, 6,0), i(STY, ZPG, 3,0), i(STA, ZPG, 3,0), i(STX, ZPG, 3,0), i(SAX, ZPG, 3,0),   i(DEY, IMP, 2,0), i(NOP, IMM, 2,0), i(TXA, IMP, 2,0), i(XAA, IMM, 2,0), i(STY, ABS, 4,0), i(STA, ABS, 4,0), i(STX, ABS, 4,0), i(SAX, ABS, 4,0), 
/* 9x */    i(BCC, REL, 2,0), i(STA, IZY, 6,0), i(KIL, IMP, 0,0), i(AHX, IZY, 6,0), i(STY, ZPX, 4,0), i(STA, ZPX, 4,0), i(STX, ZPY, 4,0), i(SAX, ZPY, 4,0),   i(TYA, IMP, 2,0), i(STA, ABY, 5,0), i(TXS, IMP, 2,0), i(TAS, ABY, 5,0), i(SHY, ABX, 5,0), i(STA, ABX, 5,0), i(SHX, ABY, 5,0), i(AHX, ABY, 5,0), 
/* Ax */    i(LDY, IMM, 2,0), i(LDA, IZX, 6,0), i(LDX, IMM, 2,0), i(LAX, IZX, 6,0), i(LDY, ZPG, 3,0), i(LDA, ZPG, 3,0), i(LDX, ZPG, 3,0), i(LAX, ZPG, 3,0),   i(TAY, IMP, 2,0), i(LDA, IMM, 2,0), i(TAX, IMP, 2,0), i(LAX, IMM, 2,0), i(LDY, ABS, 4,0), i(LDA, ABS, 4,0), i(LDX, ABS, 4,0), i(LAX, ABS, 4,0), 
/* Bx */    i(BCS, REL, 2,0), i(LDA, IZY, 5,1), i(KIL, IMP, 0,0), i(LAX, IZY, 5,1), i(LDY, ZPX, 4,0), i(LDA, ZPX, 4,0), i(LDX, ZPY, 4,0), i(LAX, ZPY, 4,0),   i(CLV, IMP, 2,0), i(LDA, ABY, 4,1), i(TSX, IMP, 2,0), i(LAS, ABY, 4,1), i(LDY, ABX, 4,1), i(LDA, ABX, 4,1), i(LDX, ABY, 4,1), i(LAX, ABY, 4,1), 
/* Cx */    i(CPY, IMM, 2,0), i(CMP, IZX, 6,0), i(NOP, IMM, 2,0), i(DCP, IZX, 8,0), i(CPY, ZPG, 3,0), i(CMP, ZPG, 3,0), i(DEC, ZPG, 5,0), i(DCP, ZPG, 5,0),   i(INY, IMP, 2,0), i(CMP, IMM, 2,0), i(DEX, IMP, 2,0), i(AXS, IMM, 2,0), i(CPY, ABS, 4,0), i(CMP, ABS, 4,0), i(DEC, ABS, 6,0), i(DCP, ABS, 6,0), 
/* Dx */    i(BNE, REL, 2,0), i(CMP, IZY, 5,1), i(KIL, IMP, 0,0), i(DCP, IZY, 8,0), i(NOP, ZPX, 4,0), i(CMP, ZPX, 4,0), i(DEC, ZPX, 6,0), i(DCP, ZPX, 6,0),   i(CLD, IMP, 2,0), i(CMP, ABY, 4,1), i(NOP, IMP, 2,0), i(DCP, ABY, 7,0), i(NOP, ABX, 4,1), i(CMP, ABX, 4,1), i(DEC, ABX, 7,0), i(DCP, ABX, 7,0), 
/* Ex */    i(CPX, IMM, 2,0), i(SBC, IZX, 6,0), i(NOP, IMM, 2,0), i(ISC, IZX, 8,0), i(CPX, ZPG, 3,0), i(SBC, ZPG, 3,0), i(INC, ZPG, 5,0), i(ISC, ZPG, 5,0),   i(INX, IMP, 2,0), i(SBC, IMM, 2,0), i(NOP, IMP, 2,0), i(SBC, IMM, 2,0), i(CPX, ABS, 4,0), i(SBC, ABS, 4,0), i(INC, ABS, 6,0), i(ISC, ABS, 6,0), 
/* Fx */    i(BEQ, REL, 2,0), i(SBC, IZY, 5,1), i(KIL, IMP, 0,0), i(ISC, IZY, 8,0), i(NOP, ZPX, 4,0), i(SBC, ZPX, 4,0), i(INC, ZPX, 6,0), i(ISC, ZPX, 6,0),   i(SED, IMP, 2,0), i(SBC, ABY, 4,1), i(NOP, IMP, 2,0), i(ISC, ABY, 7,0), i(NOP, ABX, 4,1), i(SBC, ABX, 4,1), i(INC, ABX, 7,0), i(ISC, ABX, 7,0)

};

// TODO: package status register flag setters into functions

// === Load operations ===
/// @brief Loads value at the address into the accumulator.
void CPU::LDA(WORD addr) 
{ 
	reg.accumulator = m_Bus->ReadByte(addr);
	
	reg.CheckNegative(reg.accumulator);
	reg.CheckZero(reg.accumulator);
}

/// @brief Loads value at the address into the X register.
void CPU::LDX(WORD addr) 
{ 
	reg.X = m_Bus->ReadByte(addr);
	
	reg.CheckNegative(reg.X);
	reg.CheckZero(reg.X);
}

/// @brief Loads value at the address into the Y register.
void CPU::LDY(WORD addr) 
{ 
	reg.Y = m_Bus->ReadByte(addr);
	
	reg.CheckNegative(reg.Y);
	reg.CheckZero(reg.Y);
}


// === Store operations ===
/// @brief Stores the value in the accumulator to the address.
void CPU::STA(WORD addr) 
{ 
	m_Bus->WriteByte(addr, reg.accumulator);
}

/// @brief Stores the value in the X register to the address.
void CPU::STX(WORD addr) 
{ 
	m_Bus->WriteByte(addr, reg.X);
}

/// @brief Stores the value in the Y register to the address.
void CPU::STY(WORD addr) 
{ 
	m_Bus->WriteByte(addr, reg.Y);
}


// === Increment operations ===
/// @brief Increment the stored value at a given address by one.
void CPU::INC(WORD addr) 
{ 
	BYTE val = m_Bus->ReadByte(addr);
	val++;

	reg.CheckNegative(val);
	reg.CheckZero(val);

	m_Bus->WriteByte(addr, val);
}

/// @brief Increment the value in the X register.
void CPU::INX(WORD addr) 
{ 
	reg.X++;

	reg.CheckNegative(reg.X);
	reg.CheckZero(reg.X);
}

/// @brief Increment the value in the Y register.
void CPU::INY(WORD addr) 
{ 
	reg.Y++;

	reg.CheckNegative(reg.Y);
	reg.CheckZero(reg.Y);
}


// === Decrement operations ===
/// @brief Decrement the stored value at a given address by one.
void CPU::DEC(WORD addr) 
{ 
	BYTE val = m_Bus->ReadByte(addr);
	val--;

	reg.CheckNegative(val);
	reg.CheckZero(val);
		
	m_Bus->WriteByte(addr, val);
}

/// @brief Decrement the value in the X register.
void CPU::DEX(WORD addr) 
{ 
	reg.X--;

	reg.CheckNegative(reg.X);
	reg.CheckZero(reg.X);
}

/// @brief Decrement the value in the Y register.
void CPU::DEY(WORD addr) 
{ 
	reg.Y--;

	reg.CheckNegative(reg.Y);
	reg.CheckZero(reg.Y);
}


// === Arithmetic operations ===
/// @brief Performs an addition between the accumulator and a value at the address.
///        Stores the result in the accumulator.
void CPU::ADC(WORD addr) 
{ 
	BYTE val = m_Bus->ReadByte(addr);

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
		reg.status_register.set(StatusRegisterFlags::_OVERFLOW);
	// if negative + negative = positive, overflow occured
	else if ((CHAR)reg.accumulator < 0 && (CHAR)val < 0 && (CHAR)(result) >= 0)
		reg.status_register.set(StatusRegisterFlags::_OVERFLOW);
	else
		reg.status_register.reset(StatusRegisterFlags::_OVERFLOW);
	

	reg.accumulator = result; // save result to accumulator
}

/// @brief Performs a subtraction between the accumulator and a value at the address.
///        Stores the result in the accumulator.
void CPU::SBC(WORD addr) 
{ 
	BYTE val = m_Bus->ReadByte(addr);

	WORD negated16 = reg.accumulator - val; // negate values

	// subtract only if carry bit is not set
	if (!reg.status_register.test(StatusRegisterFlags::CARRY))
		negated16--;

	BYTE result = negated16 & 0x00FF; // convert to 8-bit

	//  registers
	reg.status_register.set(StatusRegisterFlags::CARRY, negated16 < (BYTE_MAX + 1));
	reg.CheckZero(result);
	reg.CheckNegative(result);

	// determine overflow bit
	// if negative - positive = positive, overflow occured
	if ((CHAR)reg.accumulator < 0 && (CHAR)val >= 0 && (CHAR)(result) >= 0)
		reg.status_register.set(StatusRegisterFlags::_OVERFLOW);
	// if positive - negative = negative, overflow occured
	else if ((CHAR)reg.accumulator >= 0 && (CHAR)val < 0 && (CHAR)(result) < 0)
		reg.status_register.set(StatusRegisterFlags::_OVERFLOW);
	else
		reg.status_register.reset(StatusRegisterFlags::_OVERFLOW);


	reg.accumulator = result; // save result to accumulator
}

/// @brief Compares the value in the accumulator against the value read at the given address.
///        Sets the carry, zero and negative bits in the status register accordingly.
void CPU::CMP(WORD addr) 
{ 
	BYTE val = m_Bus->ReadByte(addr);
	WORD negated16 = reg.accumulator - val;

	BYTE result = negated16 & 0x00FF; // convert word to byte

	reg.CheckCarryCompare(reg.accumulator, val);
	reg.CheckZero(result);
	reg.CheckNegative(result);
}

/// @brief Compares the value in the X register against the value read at the given address.
///        Sets the carry, zero and negative bits in the status register accordingly.
void CPU::CPX(WORD addr) 
{ 
	BYTE val = m_Bus->ReadByte(addr);
	WORD negated16 = reg.X - val;

	BYTE result = negated16 & 0x00FF; // convert word to byte

	reg.CheckCarryCompare(reg.X, val);
	reg.CheckZero(result);
	reg.CheckNegative(result);
}

/// @brief Compares the value in the Y register against the value read at the given address.
///        Sets the carry, zero and negative bits in the status register accordingly.
void CPU::CPY(WORD addr) 
{ 
	BYTE val = m_Bus->ReadByte(addr);
	WORD negated16 = reg.Y - val;

	BYTE result = negated16 & 0x00FF; // convert word to byte

	reg.CheckCarryCompare(reg.Y, val);
	reg.CheckZero(result);
	reg.CheckNegative(result);
}


// === Logical operations ===
/// @brief Performs a bitwise AND operation between the accumulator and the value read at the given address.
///        Updates the accumulator with the result.
void CPU::AND(WORD addr) 
{ 
	BYTE val = m_Bus->ReadByte(addr);

	BYTE result = val & reg.accumulator;

	reg.CheckZero(result);
	reg.CheckNegative(result);

	reg.accumulator = result;
}

/// @brief Performs a bitwise XOR operation between the accumulator and the value read at the given address.
///        Updates the accumulator with the result.
void CPU::EOR(WORD addr) 
{ 
	BYTE val = m_Bus->ReadByte(addr);

	BYTE result = val ^ reg.accumulator;

	reg.CheckZero(result);
	reg.CheckNegative(result);

	reg.accumulator = result;
}

/// @brief Performs a bitwise inclusive OR operation between the accumulator and the value read at the given address.
///        Updates the accumulator with the result.
void CPU::ORA(WORD addr) 
{ 
	BYTE val = m_Bus->ReadByte(addr);

	BYTE result = val | reg.accumulator;

	reg.CheckZero(result);
	reg.CheckNegative(result);

	reg.accumulator = result;
}

/// @brief Tests if bits are in specific locations by performing a bitwise AND between the accumulator and the value
///        read at the given address.
///        Updates the zero and negative flags, and sets the overflow flag depending on the 6th bit.
void CPU::BIT(WORD addr) 
{ 
	BYTE val = m_Bus->ReadByte(addr);

	BYTE result = reg.accumulator & val;

	// http://www.6502.org/users/obelisk/6502/reference.html#BIT
	reg.CheckZero(result);
	reg.CheckNegative(val);
	// set overflow bit based on 6th bit of memory value
	reg.status_register.set(StatusRegisterFlags::_OVERFLOW, val & (1 << 6));
}


// === Stack operations ===
/// @brief Pushes the current value of the accumulator onto the stack.
void CPU::PHA(WORD addr) 
{ 
	PushStackByte(reg.accumulator);
}

/// @brief Pushes a copy of the status register onto the stack.
///        The pushed status register has the UNUSED and BREAK_COMMAND flags set.
void CPU::PHP(WORD addr) 
{ 
	auto statusCopy = reg.status_register;
	statusCopy.set(StatusRegisterFlags::UNUSED);
	statusCopy.set(StatusRegisterFlags::BREAK_COMMAND);

	PushStackByte((BYTE)(statusCopy.to_ulong()));
}

/// @brief Pops a byte from the stack into the accumulator. 
void CPU::PLA(WORD addr) 
{ 
	reg.accumulator = PopStackByte();

	reg.CheckZero(reg.accumulator);
	reg.CheckNegative(reg.accumulator);
}

/// @brief Pops a byte from the stack into the status register.
void CPU::PLP(WORD addr) 
{ 
	auto pulledStatus = PopStackByte();

	bool wasBreakSet = (reg.status_register.test(StatusRegisterFlags::BREAK_COMMAND));
	reg.status_register = pulledStatus;
	reg.status_register.set(StatusRegisterFlags::BREAK_COMMAND, wasBreakSet);
	reg.status_register.set(StatusRegisterFlags::UNUSED);
}


// === Jump operations ===
/// @brief Sets the program counter to the specified address to "jump" to that location in memory.
void CPU::JMP(WORD addr) 
{ 
	reg.program_counter = addr;
}

/// @brief Jumps to a subroutine by pushing the address of the current instruction onto the stack, and then
///        jumping to the given address.
void CPU::JSR(WORD addr) 
{ 
	reg.program_counter--;

	PushStackWord(reg.program_counter);

	JMP(addr);
}

/// @brief Returns from a subroutine by setting the program counter to a popped address from the stack.
void CPU::RTS(WORD addr) 
{ 
	reg.program_counter = PopStackWord();

	reg.program_counter++;
}


// === Branch operations ===
/*  Branch operations may take a varying number of cycles to complete:                                  *
 *   - 0 additional cycles if the branch fails to occur                                                 *
 *   - 1 additional cycle if the address to jump to is on the same page as the program counter          *
 *   - 2 additional cycles if the address to jump to is on a different page as the program counter      */

/// @brief Branches to address if the carry flag is not set.
void CPU::BCC(WORD addr) 
{ 
	if (reg.status_register.test(StatusRegisterFlags::CARRY)) 
		return;

	// we branched, so increase current cycles by at least one. if it was to a different page, increment by one
	m_curCycles += CPU::IsOnSamePage(reg.program_counter, addr) ? 1 : 2;
	
	JMP(addr);
}

/// @brief Branches to address if the carry flag is set.
void CPU::BCS(WORD addr) 
{ 
	if (!reg.status_register.test(StatusRegisterFlags::CARRY))
		return;
	
	m_curCycles += CPU::IsOnSamePage(reg.program_counter, addr) ? 1 : 2;
	
	JMP(addr);
}

/// @brief Branches to address if the zero flag is set.
void CPU::BEQ(WORD addr) 
{ 
	if (!reg.status_register.test(StatusRegisterFlags::ZERO))
		return;
		
	m_curCycles += CPU::IsOnSamePage(reg.program_counter, addr) ? 1 : 2;
	
	JMP(addr);
}

/// @brief Branches to address if the negative flag is set.
void CPU::BMI(WORD addr) 
{ 
	if (!reg.status_register.test(StatusRegisterFlags::NEGATIVE))
		return;
		
	m_curCycles += CPU::IsOnSamePage(reg.program_counter, addr) ? 1 : 2;
	
	JMP(addr);
}

/// @brief Branches to address if the zero flag is not set.
void CPU::BNE(WORD addr) 
{ 
	if (reg.status_register.test(StatusRegisterFlags::ZERO))
		return;
		
	m_curCycles += CPU::IsOnSamePage(reg.program_counter, addr) ? 1 : 2;
	
	JMP(addr);
}

/// @brief Branches to address if the negative flag is not set.
void CPU::BPL(WORD addr) 
{ 
	if (reg.status_register.test(StatusRegisterFlags::NEGATIVE))
		return;
		
	m_curCycles += CPU::IsOnSamePage(reg.program_counter, addr) ? 1 : 2;
	
	JMP(addr);
}

/// @brief Branches to address if the overflow flag is not set.
void CPU::BVC(WORD addr) 
{ 
	if (reg.status_register.test(StatusRegisterFlags::_OVERFLOW))
		return;
		
	m_curCycles += CPU::IsOnSamePage(reg.program_counter, addr) ? 1 : 2;
	
	JMP(addr);
}

/// @brief Branches to address if the overflow flag is set.
void CPU::BVS(WORD addr) 
{ 
	if (!reg.status_register.test(StatusRegisterFlags::_OVERFLOW))
		return;

	m_curCycles += CPU::IsOnSamePage(reg.program_counter, addr) ? 1 : 2;
	
	JMP(addr);
}



// === Shift operations ===
/// @brief Shifts the contents of the accumulator or memory location one bit to the left.
void CPU::ASL(WORD addr) 
{ 
	BYTE val{};
	bool isImplicit = (instructions[m_curOpcode].addrMode == &CPU::IMP);
	
	val = (isImplicit ? reg.accumulator : m_Bus->ReadByte(addr));

	// set carry flag if 7th bit (the one we're about to shift away) is set
	reg.status_register.set(StatusRegisterFlags::CARRY, val & 0x80);

	BYTE result = val << 1;
	reg.CheckNegative(result);
	reg.CheckZero(result);
	

	if (isImplicit)
		reg.accumulator = result;
	else
		m_Bus->WriteByte(addr, result); // write value back to same location
}

/// @brief Shifts the contents of the accumulator or memory location one bit to the right.
void CPU::LSR(WORD addr) 
{ 
	BYTE val{};
	bool isImplicit = (instructions[m_curOpcode].addrMode == &CPU::IMP);

	val = (isImplicit ? reg.accumulator : m_Bus->ReadByte(addr));

	// set carry flag if 1st bit (the one we're about to shift away) is set
	reg.status_register.set(StatusRegisterFlags::CARRY, val & 0x01);

	BYTE result = val >> 1;
	reg.CheckNegative(result);
	reg.CheckZero(result);
	

	if (isImplicit)
		reg.accumulator = result;
	else
		m_Bus->WriteByte(addr, result); // write value back to same location
}

/// @brief Rotates the contents of the accumulator or memory location to the left through the carry flag.
void CPU::ROL(WORD addr) 
{ 
	BYTE val{};
	bool isImplicit = (instructions[m_curOpcode].addrMode == &CPU::IMP);
	
	val = (isImplicit ? reg.accumulator : m_Bus->ReadByte(addr));

	// shift to left while setting bit 0 to value of carry flag
	BYTE result = (val << 1) | reg.status_register.test(StatusRegisterFlags::CARRY);

	// set carry flag if 7th bit (the one we're about to shift away) is set
	reg.status_register.set(StatusRegisterFlags::CARRY, val & 0x80);

	reg.CheckNegative(result);
	reg.CheckZero(result);
	

	if (isImplicit)
		reg.accumulator = result;
	else
		m_Bus->WriteByte(addr, result); // write value back to same location
}

/// @brief Rotates the contents of the accumulator or memory location to the right through the carry flag.
void CPU::ROR(WORD addr) 
{ 
	BYTE val{};
	bool isImplicit = (instructions[m_curOpcode].addrMode == &CPU::IMP);
	
	val = (isImplicit ? reg.accumulator : m_Bus->ReadByte(addr));

	// shift to left while setting bit 7 to value of carry flag
	BYTE result = (val >> 1) | (reg.status_register.test(StatusRegisterFlags::CARRY) << 7);
	
	// set carry flag if 1st bit (the one we're about to shift away) is set
	reg.status_register.set(StatusRegisterFlags::CARRY, val & 0x01);

	reg.CheckNegative(result);
	reg.CheckZero(result);
	

	if (isImplicit)
		reg.accumulator = result;
	else
		m_Bus->WriteByte(addr, result); // write value back to same location
}


// === Clear flag operations ===
/// @brief Resets the carry flag.
void CPU::CLC(WORD addr) 
{ 
	reg.status_register.reset(StatusRegisterFlags::CARRY);
}

/// @brief Resets the decimal mode flag.
void CPU::CLD(WORD addr) 
{ 
	reg.status_register.reset(StatusRegisterFlags::DECIMAL_MODE);
}

/// @brief Resets the IRQ flag.
void CPU::CLI(WORD addr) 
{ 
	reg.status_register.reset(StatusRegisterFlags::INTERRUPT_REQUEST);
}

/// @brief Resets the overflow flag.
void CPU::CLV(WORD addr) 
{ 
	reg.status_register.reset(StatusRegisterFlags::_OVERFLOW);
}


// === Set flag operations ===
/// @brief Sets the carry flag.
void CPU::SEC(WORD addr) 
{ 
	reg.status_register.set(StatusRegisterFlags::CARRY);
}

/// @brief Sets the decimal mode flag.
void CPU::SED(WORD addr) 
{ 
	reg.status_register.set(StatusRegisterFlags::DECIMAL_MODE);
}

/// @brief Sets the IRQ flag.
void CPU::SEI(WORD addr) 
{ 
	reg.status_register.set(StatusRegisterFlags::INTERRUPT_REQUEST);
}


// === Transfer operations ===
/// @brief Sets the X register to the value in the accumulator.
void CPU::TAX(WORD addr) 
{ 
	reg.X = reg.accumulator;

	reg.CheckNegative(reg.X);
	reg.CheckZero(reg.X);
}

/// @brief Sets the Y register to the value in the accumulator.
void CPU::TAY(WORD addr) 
{ 
	reg.Y = reg.accumulator;

	reg.CheckNegative(reg.Y);
	reg.CheckZero(reg.Y);
}

/// @brief Sets the accumulator to the value in the X register.
void CPU::TXA(WORD addr) 
{ 
	reg.accumulator = reg.X;

	reg.CheckNegative(reg.accumulator);
	reg.CheckZero(reg.accumulator);
}

/// @brief Sets the accumulator to the value in the Y register.
void CPU::TYA(WORD addr) 
{ 
	reg.accumulator = reg.Y;

	reg.CheckNegative(reg.Y);
	reg.CheckZero(reg.Y);
}

/// @brief Sets the X register to the value in the stack pointer.
void CPU::TSX(WORD addr) 
{ 
	reg.X = reg.stack_pointer;

	reg.CheckZero(reg.X);
	reg.CheckNegative(reg.X);
}

/// @brief Sets the stack pointer to the value in the X register.
void CPU::TXS(WORD addr) 
{ 
	reg.stack_pointer = reg.X;
}


// === System functions ===
/// @brief Used to generate a software interrupt. Pushes the address of the next instruction onto the
///        stack, then pushes the current status register (with the UNUSED and BREAK flags set) onto the stack.
///        It then jumps to the address read at the interrupt vector (usually 0xFFFE to 0xFFFF).
void CPU::BRK(WORD addr) 
{ 
	reg.program_counter++;

	auto savedReg = reg.status_register;
	savedReg.set(StatusRegisterFlags::UNUSED); // "always pushed as 1" https://www.nesdev.org/wiki/Status_flags#Flags
	savedReg.set(StatusRegisterFlags::BREAK_COMMAND); // https://www.nesdev.org/wiki/Status_flags#The_B_flag

	PushStackWord(reg.program_counter);
	PushStackByte((BYTE)(savedReg.to_ulong()));

	reg.status_register.set(StatusRegisterFlags::INTERRUPT_REQUEST);

	reg.program_counter = m_Bus->ReadWord(IRQ_VECTOR);
}

/// @brief No operation - does nothing but consume CPU cycles.
void CPU::NOP(WORD addr) 
{ 
	//
}

/// @brief Used to return control flow to the main program after an interrupt routine has completed. Pops the status register
///        off the the stack (with the UNUSED and BREAK flags reset), and then pops the program counter off the stack.
void CPU::RTI(WORD addr) 
{
	reg.status_register = PopStackByte();
	reg.status_register.set(StatusRegisterFlags::UNUSED);
	reg.status_register.reset(StatusRegisterFlags::BREAK_COMMAND);

	reg.program_counter = PopStackWord();
}


// === Illegal/unused operations ===
// https://www.nesdev.org/wiki/Programming_with_unofficial_opcodes
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
	LDA(addr);
	TAX(addr);
}

void CPU::KIL(WORD addr) 
{ 

}

void CPU::SLO(WORD addr) 
{ 
	ASL(addr);
	ORA(addr);
}

void CPU::RLA(WORD addr) 
{ 
	ROL(addr);
	AND(addr);
}

void CPU::SRE(WORD addr) 
{ 
	LSR(addr);
	EOR(addr);
}

void CPU::RRA(WORD addr) 
{ 
	ROR(addr);
	ADC(addr);
}

void CPU::SAX(WORD addr) 
{ 
	BYTE result = reg.X & reg.accumulator;

	m_Bus->WriteByte(addr, result);
}

void CPU::DCP(WORD addr) 
{ 
	DEC(addr);
	CMP(addr);
}

void CPU::ANC(WORD addr) 
{ 
	AND(addr);
	// copy negative flag to carry
	reg.status_register.set(StatusRegisterFlags::CARRY, StatusRegisterFlags::NEGATIVE);
}

void CPU::ALR(WORD addr) // ASR
{ 
	AND(addr);
	LSR(addr);
}

void CPU::ARR(WORD addr) 
{ 
	AND(addr);
	ROR(addr);

	// set carry based on 6th bit
	reg.status_register.set(StatusRegisterFlags::CARRY, reg.accumulator & (1 << 6));
	// set overflow based on bit 5 XOR bit 6
	reg.status_register.set(StatusRegisterFlags::_OVERFLOW, (reg.accumulator & (1 << 5)) ^ (reg.accumulator & (1 << 6)));
}

void CPU::LAS(WORD addr) 
{ 

}

void CPU::AXS(WORD addr) 
{ 
	BYTE val = m_Bus->ReadByte(addr);
	BYTE result = (reg.X & reg.accumulator) - val;

	reg.status_register.set(StatusRegisterFlags::CARRY, val <= (reg.X & reg.accumulator));
	reg.CheckNegative(result);
	reg.CheckZero(result);
}

void CPU::ISC(WORD addr) // ISB
{ 
	INC(addr);
	SBC(addr);
}