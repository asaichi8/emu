#pragma once

#include "IPPURegister.h"


class PPUSCROLL : public IPPURegister
{
public:
    PPUSCROLL() : IPPURegister((WORD)Registers::PPUSCROLL) {}
    ~PPUSCROLL() override {}
};