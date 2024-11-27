#pragma once

#include "IPPURegister.h"


class OAMADDR : public IPPURegister
{
public:
    OAMADDR() : IPPURegister((WORD)Registers::OAMADDR) {}
    ~OAMADDR() override {}
};