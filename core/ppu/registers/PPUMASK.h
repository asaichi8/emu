#pragma once

#include "IPPURegister.h"


class PPUMASK : public IPPURegister
{
public:
    PPUMASK() : IPPURegister((WORD)Registers::PPUMASK) {}
    ~PPUMASK() override {}
};