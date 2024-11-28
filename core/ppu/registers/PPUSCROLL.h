#pragma once

#include "IPPURegister.h"


class PPUSCROLL : public IPPURegister
{
public:
    PPUSCROLL(InternalRegisters* _internal_registers) : IPPURegister((WORD)PPURegAddr::PPUSCROLL, _internal_registers) {}
    ~PPUSCROLL() override {}

    void Write(std::bitset<8> val) override { internal_registers->w = !internal_registers->w; }
};