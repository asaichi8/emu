#pragma once

#include "IPPURegister.h"


class OAMDATA : public IPPURegister
{
public:
    OAMDATA() : IPPURegister((WORD)PPURegAddr::OAMDATA) {}
    ~OAMDATA() override {}
};