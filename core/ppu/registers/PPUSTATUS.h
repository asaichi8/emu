#pragma once

#include "IPPURegister.h"


class PPUSTATUS : public IPPURegister
{
    std::bitset<8> ppu_status_register;

public:
    PPUSTATUS(InternalRegisters* _internal_registers) : IPPURegister((WORD)PPURegAddr::PPUSTATUS, _internal_registers) {}
    ~PPUSTATUS() override {}

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
        VBLANK // cleared on PPUSTATUS read - https://www.nesdev.org/wiki/PPU_registers#Vblank_flag
    };

    //std::bitset<8>* GetRegVal() override { return &ppu_status_register; }
    std::bitset<8> Read() override 
    { 
        auto preservedReg = ppu_status_register; // save data before affecting flags

        ppu_status_register.reset(PPUStatusRegisterFlags::VBLANK);
        internal_registers->w = false;

        return preservedReg;
    }
    void SetSprite0Hit(bool b) { ppu_status_register.set(SPRITE_0_HIT, b); }    
    void SetVBLANK(bool b) { ppu_status_register.set(VBLANK, b); }    
};