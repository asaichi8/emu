#pragma once

#include <bitset>
#include "typedefs.h"


// "Flags of the status register" - http://www.oxyron.de/html/opcodes02.html
enum StatusRegisterFlags
{
	CARRY,              // C - 1 on unsigned overflow
	ZERO,               // Z - 1 if result is equal to zero
	INTERRUPT_REQUEST,  // I - 1 when disabling interrupt requests
	DECIMAL_MODE,       // D - 1 when in BCD mode (unused in NES implementation)
	BREAK_COMMAND,      // B - 1 when a software interrupt is executed
	UNUSED,             // # - 1 always
	OVERFLOW,           // V - 1 if arithmetic operation overflows
	NEGATIVE            // N - 1 if result is negative (two's complement)
};

class CPURegisters
{
public:
	WORD program_counter{};             // 16-bit address denoting the position of currently executed instruction
	BYTE accumulator{};                 // 8-bit value acting as an index register, often in conjunction with arithmetic operations
	BYTE X{};                           // 8-bit value acting as an index register
	BYTE Y{};                           // 8-bit value acting as an index register
	BYTE stack_pointer{};               // 8-bit address pointing to the top of the stack
	std::bitset<8> status_register{};   // 8-bit value holding flags that represent the current state of the CPU (see StatusRegisterFlags)

	void CheckZero(BYTE val);
	void CheckNegative(BYTE val);
	void CheckCarry(WORD val);
	void CheckCarryCompare(BYTE regVal, BYTE val) ;
};