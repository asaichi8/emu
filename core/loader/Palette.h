#pragma once

#include "Loader.h"

#define PALETTE_SIZE 64


#pragma pack(push, 1) // ensure no padding, so sizeof(RGB) == 3
struct RGB
{
    BYTE r = 0;
    BYTE g = 0;
    BYTE b = 0;
};
#pragma pack(pop)


class Palette : public Loader
{
    std::vector<RGB> curPalette;

public:
    Palette();
};