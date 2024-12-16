#pragma once

#include "IPPURegister.h"


// TODO: this is unused, probably delete it
// $4014
class OAMDMA : public IPPURegister
{
public:
    OAMDMA(InternalRegisters* _internal_registers) : IPPURegister((WORD)PPURegAddr::OAMDMA, _internal_registers) {}
    ~OAMDMA() override {}
};