#pragma once

#include "IPPURegister.h"


class PPUSTATUS : public IPPURegister
{
public:
    PPUSTATUS() : IPPURegister((WORD)PPURegAddr::PPUSTATUS) {}
    ~PPUSTATUS() override {}
};