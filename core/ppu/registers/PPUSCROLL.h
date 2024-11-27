#pragma once

#include "IPPURegister.h"


class PPUSCROLL : public IPPURegister
{
public:
    PPUSCROLL() : IPPURegister((WORD)PPURegAddr::PPUSCROLL) {}
    ~PPUSCROLL() override {}
};