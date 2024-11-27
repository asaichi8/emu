#pragma once

#include "IPPURegister.h"


class OAMDMA : public IPPURegister
{
public:
    OAMDMA() : IPPURegister((WORD)PPURegAddr::OAMDMA) {}
    ~OAMDMA() override {}
};