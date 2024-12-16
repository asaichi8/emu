#pragma once

#include "IPPURegister.h"


// TODO: this is unused, probably delete it
// $2004
class OAMDATA : public IPPURegister
{
public:
    OAMDATA(InternalRegisters* _internal_registers) : IPPURegister((WORD)PPURegAddr::OAMDATA, _internal_registers) {}
    ~OAMDATA() override {}
};