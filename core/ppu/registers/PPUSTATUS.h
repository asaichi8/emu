#pragma once

#include "IPPURegister.h"


class PPUSTATUS : public IPPURegister
{
    // https://www.nesdev.org/wiki/PPU_registers#PPUSTATUS_-_Rendering_events_($2002_read)
    enum PPUStatusRegisterFlags
    {
        PPUIDENTIFIER0, // PPU identifier seems to be used for copy protection
        PPUIDENTIFIER1,
        PPUIDENTIFIER2,
        PPUIDENTIFIER3,
        PPUIDENTIFIER4,
        SPRITE_OVERFLOW,
        SPRITE_0_HIT,
        VBLANK
    };

public:
    PPUSTATUS() : IPPURegister((WORD)PPURegAddr::PPUSTATUS) {}
    ~PPUSTATUS() override {}

    std::bitset<8> ppu_status_register; // TODO: can/should this be abstracted further?
};