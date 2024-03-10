#include "CPURegisters.h"

/// @brief Checks if the value is zero, and sets register based on result
void CPURegisters::CheckZero(BYTE val)
{
	this->status_register.set(StatusRegisterFlags::ZERO, val == 0x00);
}

/// @brief Checks if the value is negative, and sets register based on result
void CPURegisters::CheckNegative(BYTE val)
{
	this->status_register.set(StatusRegisterFlags::NEGATIVE, (CHAR)val < 0);
}

/// @brief Checks if the value is has a carry, and sets register based on result
void CPURegisters::CheckCarry(WORD val)
{
	// set carry flag if we don't need to borrow, reset otherwise
	this->status_register.set(StatusRegisterFlags::CARRY, val > BYTE_MAX);
}

/// @brief In the comparison operations, the carry bit is set if the register value is greater
///        than the read value.
void CPURegisters::CheckCarryCompare(BYTE regVal, BYTE val) 
{
	this->status_register.set(StatusRegisterFlags::CARRY, regVal >= val);
}