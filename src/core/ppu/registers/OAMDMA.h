#pragma once

#include "IPPURegister.h"


class OAMDMA : public IPPURegister
{
public:
    OAMDMA(InternalRegisters* _internal_registers) : IPPURegister((WORD)PPURegAddr::OAMDMA, _internal_registers) {}
    ~OAMDMA() override {}
};