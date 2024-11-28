#pragma once

#include "IPPURegister.h"


class OAMADDR : public IPPURegister
{
public:
    OAMADDR(InternalRegisters* _internal_registers) : IPPURegister((WORD)PPURegAddr::OAMADDR, _internal_registers) {}
    ~OAMADDR() override {}
};