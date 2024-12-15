#pragma once

#include "IPPURegister.h"


class PPUSCROLL : public IPPURegister
{
    BYTE x = 0;
    BYTE y = 0;

public:
    PPUSCROLL(InternalRegisters* _internal_registers) : IPPURegister((WORD)PPURegAddr::PPUSCROLL, _internal_registers) {}
    ~PPUSCROLL() override {}

    void Write(std::bitset<8> val) override 
    {
        if (internal_registers->w)
            y = val.to_ulong();
        else
            x = val.to_ulong();

        internal_registers->w = !internal_registers->w; 
    }

    BYTE GetX() { return x; }
    BYTE GetY() { return y; }
};