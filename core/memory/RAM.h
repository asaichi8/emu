#pragma once

#include <memory>
#include "typedefs.h"


class RAM
{
    size_t m_RAM_Size{};
    
protected:
    std::unique_ptr<BYTE[]> m_RAM{};

public:
    RAM(size_t size);
    ~RAM();

    void Reset();

     BYTE ReadByte(WORD addr);
     WORD ReadWord(WORD addr, bool shouldWrapPage = false);
     virtual void WriteByte(WORD addr, BYTE val);
     virtual void WriteWord(WORD addr, WORD val);
};