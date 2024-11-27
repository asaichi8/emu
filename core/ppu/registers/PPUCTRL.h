#pragma once

#include "IPPURegister.h"


class PPUCTRL : public IPPURegister
{
public:
    PPUCTRL() : IPPURegister((WORD)PPURegAddr::PPUCTRL) {}
    ~PPUCTRL() override {}
};