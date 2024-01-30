#include "CPURegisters.h"

// Checks if the value is zero, and sets register based on result
void CPURegisters::CheckZero(BYTE val)
{
    if (val == 0x00)
        this->status_register.set(StatusRegisterFlags::ZERO);
    else
        this->status_register.reset(StatusRegisterFlags::ZERO);
}

// Checks if the value is negative, and sets register based on result
void CPURegisters::CheckNegative(BYTE val)
{
    if ((CHAR)val < 0)
        this->status_register.set(StatusRegisterFlags::NEGATIVE);
    else
        this->status_register.reset(StatusRegisterFlags::NEGATIVE);
}

// Checks if the value is has a carry, and sets register based on result
void CPURegisters::CheckCarry(WORD val)
{
    // set carry flag if we don't need to borrow, reset otherwise
    if (val > BYTE_MAX)
        this->status_register.set(StatusRegisterFlags::CARRY);
    else
        this->status_register.reset(StatusRegisterFlags::CARRY);
}

void CPURegisters::CheckCarryCompare(BYTE regVal, BYTE val) 
{
    if (regVal >= val)
        this->status_register.set(StatusRegisterFlags::CARRY);
    else
        this->status_register.reset(StatusRegisterFlags::CARRY);
}