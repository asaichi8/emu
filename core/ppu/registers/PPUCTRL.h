#pragma once

#include "IPPURegister.h"


class PPUCTRL : public IPPURegister
{
public:
    PPUCTRL() : IPPURegister((WORD)Registers::PPUCTRL) {}
    ~PPUCTRL() override {}
};