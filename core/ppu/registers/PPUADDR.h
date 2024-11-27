#pragma once

#include "IPPURegister.h"


class PPUADDR : public IPPURegister
{
public:
    PPUADDR() : IPPURegister((WORD)Registers::PPUADDR) {}
    ~PPUADDR() override {}
};