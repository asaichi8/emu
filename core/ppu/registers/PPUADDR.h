#pragma once

#include "IPPURegister.h"


class PPUADDR : public IPPURegister
{
public:
    PPUADDR(InternalRegisters* _internal_registers) : IPPURegister((WORD)PPURegAddr::PPUADDR, _internal_registers) {}
    ~PPUADDR() override {}
};