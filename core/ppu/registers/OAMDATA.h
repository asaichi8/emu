#pragma once

#include "IPPURegister.h"


class OAMDATA : public IPPURegister
{
public:
    OAMDATA(InternalRegisters* _internal_registers) : IPPURegister((WORD)PPURegAddr::OAMDATA, _internal_registers) {}
    ~OAMDATA() override {}
};