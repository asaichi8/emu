#include "../memory/RAM.h"

class PPU
{
    RAM* m_RAM = nullptr;

public:
    PPU(RAM* ram);
};