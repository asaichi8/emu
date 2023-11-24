#pragma once

#include "../../include/typedefs.h"

#define MEMORY_MAX UINT16_MAX + 1

class RAM
{
    BYTE m_RAM[MEMORY_MAX]{};

public:
    RAM();

    void Reset();

    BYTE ReadByte(WORD addr);
    WORD ReadWord(WORD addr);
    void WriteByte(WORD addr, BYTE val);
    void WriteWord(WORD addr, WORD val);
};