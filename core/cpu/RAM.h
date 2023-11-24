#pragma once

#include "../../include/typedefs.h"

#define MEMORY_MAX UINT16_MAX + 1

class RAM
{

public:
    RAM();

    BYTE ram[MEMORY_MAX]{};
};