#pragma once

#include <memory>
#include "../../include/typedefs.h"


class RAM
{
    std::unique_ptr<BYTE[]> m_RAM{};
    size_t m_RAM_Size{};

public:
    RAM(size_t size);
    ~RAM();

    void Reset();

    BYTE ReadByte(WORD addr);
    WORD ReadWord(WORD addr, bool shouldWrapPage = false);
    void WriteByte(WORD addr, BYTE val);
    void WriteWord(WORD addr, WORD val);
};