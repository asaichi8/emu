#pragma once

#include <bitset>
#include "../../include/typedefs.h"

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

class cpu
{
    BYTE ram[UINT16_MAX + 1];

    struct registers
    {
        WORD program_counter{};
        BYTE accumulator{};
        BYTE X{};   
        BYTE Y{};
        BYTE stack_pointer{};
        std::bitset<8> status_register{};
    };



public:
    cpu();
    
    registers reg;

};