#pragma once

#include <bitset>
#include "RAM.h"
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
    };

    void Reset();

public:
    CPU(RAM* ram);
    
    registers reg;
};