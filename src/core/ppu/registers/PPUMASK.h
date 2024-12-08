#pragma once

#include "IPPURegister.h"


class PPUMASK : public IPPURegister
{
    std::bitset<8> ppu_mask_register{};

public:
    PPUMASK(InternalRegisters* _internal_registers) : IPPURegister((WORD)PPURegAddr::PPUMASK, _internal_registers) {}
    ~PPUMASK() override {}

    // https://www.nesdev.org/wiki/PPU_registers#PPUMASK_-_Rendering_settings_($2001_write)
    enum PPUMaskRegisterFlags
    {
        GREYSCALE,
        LEFT_BACKGROUND, // leftmost 8 pixels
        LEFT_SPRITES,    // leftmost 8 pixels
        BACKGROUND,
        SPRITES,         
        EMPHASISE_RED,          // green on PAL
        EMPHASISE_GREEN,        // red on PAL
        EMPHASISE_BLUE,
    };

    //std::bitset<8>* GetRegVal() override { return &ppu_mask_register; }
    void Write(std::bitset<8> val) override { ppu_mask_register = val; }

    bool IsGrayscale()             { return ppu_mask_register.test(GREYSCALE); }
    bool IsLeftBackgroundShowing() { return ppu_mask_register.test(LEFT_BACKGROUND); }
    bool IsLeftSpritesShowing()    { return ppu_mask_register.test(LEFT_SPRITES); }
    bool IsBackgroundShowing()     { return ppu_mask_register.test(BACKGROUND); }
    bool IsSpritesShowing()        { return ppu_mask_register.test(SPRITES); }
    bool GetRedEmphasis()          { return ppu_mask_register.test(EMPHASISE_RED); }
    bool GetGreenEmphasis()        { return ppu_mask_register.test(EMPHASISE_BLUE); }
    bool GetBlueEmphasis()         { return ppu_mask_register.test(EMPHASISE_BLUE); }
};