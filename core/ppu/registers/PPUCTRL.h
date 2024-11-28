#pragma once

#include "IPPURegister.h"


class PPUCTRL : public IPPURegister
{
    std::bitset<8> ppu_ctrl_register;

public:
    PPUCTRL(InternalRegisters* _internal_registers) : IPPURegister((WORD)PPURegAddr::PPUCTRL, _internal_registers) {}
    ~PPUCTRL() override {}

    // https://www.nesdev.org/wiki/PPU_registers#PPUCTRL_-_Miscellaneous_settings_($2000_write)
    enum PPUCtrlRegisterFlags
    {
        NAMETABLE_SCROLL_X, // https://www.nesdev.org/wiki/PPU_registers#Scrolling
        NAMETABLE_SCROLL_Y,
        VRAM_ADDR_INCREMENT,
        SPRITE_PTABLE_ADDR,
        BACKGROUND_PTABLE_ADDR,
        SPRITE_SIZE,
        MASTER_SLAVE_SELECT,
        VBLANK_NMI
    };

    //std::bitset<8>* GetRegVal() override { return &ppu_ctrl_register; }
    void Write(std::bitset<8> val) override { ppu_ctrl_register = val; }
};