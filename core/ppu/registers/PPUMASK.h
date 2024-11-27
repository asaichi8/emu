#pragma once

#include "IPPURegister.h"


class PPUMASK : public IPPURegister
{
public:
    PPUMASK() : IPPURegister((WORD)PPURegAddr::PPUMASK) {}
    ~PPUMASK() override {}
};