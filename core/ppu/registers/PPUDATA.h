#pragma once

#include "IPPURegister.h"


class PPUDATA : public IPPURegister
{
public:
    PPUDATA() : IPPURegister((WORD)Registers::PPUDATA) {}
    ~PPUDATA() override {}
};