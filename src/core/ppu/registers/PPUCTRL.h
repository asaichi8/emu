#pragma once

#include "IPPURegister.h"


class PPUCTRL : public IPPURegister
{
    std::bitset<8> ppu_ctrl_register{};

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

    WORD GetNametableAddr()
    {
        // return 0x2000 * ((ppu_ctrl_register.to_ulong() & 0b00000011) * 0x400)
        switch (ppu_ctrl_register.to_ulong() & 0b00000011) // switch bottom two bits : 0-3
        {
            case 0:
                return 0x2000;
            case 1:
                return 0x2400;
            case 2:
                return 0x2800;
            case 3:
                return 0x2C00;
        }
    }
    BYTE GetVRAMAddrIncrement()    { return ppu_ctrl_register.test(VRAM_ADDR_INCREMENT) ? 32 : 1; }
    WORD GetSpritePTableAddr()     { return ppu_ctrl_register.test(SPRITE_PTABLE_ADDR) ? 0x1000 : 0x0000; }
    WORD GetBackgroundPTableAddr() { return ppu_ctrl_register.test(BACKGROUND_PTABLE_ADDR) ? 0x1000 : 0x0000; }
    BYTE GetSpriteSize()           { return ppu_ctrl_register.test(SPRITE_SIZE) ? 16 : 8; }
    bool GetMasterSlaveSelect()    { return ppu_ctrl_register.test(MASTER_SLAVE_SELECT); }
    bool GetVBlankNMI()            { return ppu_ctrl_register.test(VBLANK_NMI); }
};