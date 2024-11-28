#pragma once

#include "IPPURegister.h"


class PPUDATA : public IPPURegister
{
public:
    PPUDATA(InternalRegisters* _internal_registers) : IPPURegister((WORD)PPURegAddr::PPUDATA, _internal_registers) {}
    ~PPUDATA() override {}
    
    void Write(std::bitset<8> val) override { internal_registers->w = !internal_registers->w; }
};