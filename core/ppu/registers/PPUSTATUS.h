#pragma once

#include "IPPURegister.h"


class PPUSTATUS : public IPPURegister
{
public:
    PPUSTATUS() : IPPURegister((WORD)Registers::PPUSTATUS) {}
    ~PPUSTATUS() override {}
};