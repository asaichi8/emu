#include "PPU.h"


PPU::PPU(std::vector<BYTE>* _pCHR_ROM, MirrorType* _pMirrorType) : m_pCHR_ROM(_pCHR_ROM), m_pMirrorType(_pMirrorType), registers(&internal_registers) 
{
    for (auto& pTable : m_NametableRAM)
        pTable.assign(NAMETABLE_SIZE, 0);
    m_OAM.assign(PAGE, 0); // https://www.nesdev.org/wiki/PPU_memory_map#OAM
    m_PaletteRAM.assign(PALETTE_RAM_TOTAL_SIZE, 0);
    
    // if (m_pCHR_ROM->empty())
    //     m_pCHR_ROM->assign(0x2000, 0);
}


// https://www.nesdev.org/wiki/PPU_rendering
bool PPU::Clock(DWORD nCycles)
{
    const size_t SCANLINE_END = 341;
    const size_t VBLANK_START = 241;
    const size_t VBLANK_END   = 260;

    m_nPPUCycles += nCycles;

    if (m_nPPUCycles < SCANLINE_END)
        return false;
    
    // emulate sprite zero hits : https://forums.nesdev.org/viewtopic.php?t=10272
    PPUSTATUS* ppuStatusRegister = dynamic_cast<PPUSTATUS*>(registers.ppustatus.get());
    PPUMASK* ppuMaskRegister = dynamic_cast<PPUMASK*>(registers.ppumask.get());

    const SpriteData* pZeroSprite = (const SpriteData *)(&(m_OAM.at(0)));
    if (ppuMaskRegister->IsSpritesShowing()) // only hit if sprite is showing
        if (pZeroSprite->tileX <= m_nPPUCycles && pZeroSprite->tileY == m_nScanlines) // check if ppu has rendered up to zeroth sprite
            ppuStatusRegister->SetSprite0Hit(1);
    
    m_nPPUCycles -= SCANLINE_END;
    m_nScanlines++;

    if (m_nScanlines == VBLANK_START)
    {
        ppuStatusRegister->SetVBLANK(1);
        ppuStatusRegister->SetSprite0Hit(0);
    
        PPUCTRL* ppuCtrlRegister = dynamic_cast<PPUCTRL*>(registers.ppuctrl.get());
        if (ppuCtrlRegister->GetVBlankNMI())
            m_bShouldNMIInterrupt = true;
    }
    else if (m_nScanlines > (VBLANK_END + 1))
    {
        // frame complete
        ppuStatusRegister->SetVBLANK(0);
        ppuStatusRegister->SetSprite0Hit(0);
        m_nScanlines = 0;
        m_bShouldNMIInterrupt = false;

        return true;
    }

    return false;
}

void PPU::PPUCtrlWriteW(BYTE val)
{
    PPUSTATUS* ppuStatusRegister = dynamic_cast<PPUSTATUS*>(registers.ppustatus.get());
    PPUCTRL* ppuCtrlRegister = dynamic_cast<PPUCTRL*>(registers.ppuctrl.get());
    
    auto vblankBefore = ppuCtrlRegister->GetVBlankNMI();
    registers.ppuctrl->Write(val);
    
    if (ppuStatusRegister->GetVBLANK() && ppuCtrlRegister->GetVBlankNMI() && !vblankBefore)
        m_bShouldNMIInterrupt = true;
}

bool PPU::IsNMIInterruptQueued()
{
    bool isQueued = m_bShouldNMIInterrupt;
    m_bShouldNMIInterrupt = false;
    return isQueued;
}

BYTE PPU::ReadPPUByte()
{
    BYTE data = 0x0;
    WORD addr = GetMirroredPPUADDRAddress(true);

    if (addr < PATTERN_TABLES_END) // pattern table 0x0 - 0x1FFF
    {
        PPUDATA* ppuDataRegister = dynamic_cast<PPUDATA*>(registers.ppudata.get());
        data = ppuDataRegister->Read().to_ulong();
        
        ppuDataRegister->Write(m_pCHR_ROM->at(addr));
    }
    else if (addr < NAMETABLES_MIRRORED_END) // nametable 0x2000 - 0x3EFF
    {
        PPUDATA* ppuDataRegister = dynamic_cast<PPUDATA*>(registers.ppudata.get());
        data = ppuDataRegister->Read().to_ulong();

        auto index = GetNametableIndex(addr);
        addr = Bus::MirrorAddress(addr, NAMETABLES_TOTAL_SIZE); // mirror to 0x0000 - 0x0FFF
        addr = Bus::MirrorAddress(addr, NAMETABLE_SIZE); // mirror to 0x0000 - 0x0400

        ppuDataRegister->Write(m_NametableRAM[index][addr]);
    }
    else if (addr < PPU_ADDRESS_SPACE_END) // palette ram 0x3F00 - 0x3FFF
    {
        addr = MirrorPaletteRAMAddress(addr);
        if (addr >= PALETTE_RAM_BEGIN + PALETTE_RAM_TOTAL_SIZE)
            LOG_ERROR("Failed to mirror palette ram address!");
        data = m_PaletteRAM.at(addr - PALETTE_RAM_BEGIN);
    }
    else 
        LOG_WARN("Invalid PPU read (this should never occur)");

    return data;
}

void PPU::WritePPUByte(BYTE val)
{
    WORD addr = GetMirroredPPUADDRAddress(true);
    
    // pattern table 0x0 - 0x1FFF and nametable 0x2000 - 3EFF
    // writing to 0x0 - 0x1FFF indicates writing to CHR ROM using CHR RAM
    if (addr < NAMETABLES_MIRRORED_END) 
    {
        auto index = GetNametableIndex(addr);
        addr = Bus::MirrorAddress(addr, NAMETABLES_TOTAL_SIZE); // mirror to 0x0000 - 0x0FFF
        addr = Bus::MirrorAddress(addr, NAMETABLE_SIZE); // mirror to 0x0000 - 0x0400
        m_NametableRAM[index][addr] = val;
    }
    else if (addr < PALETTE_RAM_MIRRORED_END) // palette ram 0x3F00 - 0x3FFF
    {
        addr = MirrorPaletteRAMAddress(addr);
        if (addr >= PALETTE_RAM_BEGIN + PALETTE_RAM_TOTAL_SIZE)
            LOG_ERROR("Failed to mirror palette ram address");
        m_PaletteRAM.at(addr - PALETTE_RAM_BEGIN) = val;
    }
}


WORD PPU::GetMirroredPPUADDRAddress(bool shouldIncrement)
{
    // get address & mirror
    PPUADDR* ppuAddrRegister = dynamic_cast<PPUADDR*>(registers.ppuaddr.get());
    WORD addr = ppuAddrRegister->GetAddress();
    if (addr >= 0x4000)
        LOG_WARN("addr was " << addr << ", should never occur");
    addr = Bus::MirrorAddress(addr, PPU_ADDRESS_SPACE_END); // mirror 0x0 - 0x3FFF

    // increment address after reading
    if (shouldIncrement)
    {
        PPUCTRL* ppuCtrlRegister = dynamic_cast<PPUCTRL*>(registers.ppuctrl.get());
        ppuAddrRegister->IncrementAddr(ppuCtrlRegister->GetVRAMAddrIncrement()); 
    }

    return addr;
}

WORD PPU::MirrorPaletteRAMAddress(WORD addr)
{
    if (addr < PALETTE_RAM_BEGIN || addr >= PALETTE_RAM_MIRRORED_END)
    {
        LOG_WARN("Attempted to palette ram mirror \"Palette RAM\" address 0x" << std::hex << addr << std::dec);
        return addr;
    }

    addr = Bus::MirrorAddress(addr, PALETTE_RAM_TOTAL_SIZE, PALETTE_RAM_BEGIN); 

    // palette ram 0x3F00 - 0x3F20
    /* https://www.nesdev.org/wiki/PPU_palettes#Palette_RAM :
        "Note that entry 0 of each palette is also unique in that its color value is shared between the
        background and sprite palettes, so writing to either one updates the same internal storage. "    */ 
    for (WORD spritePaletteTblEntryZero = PALETTE_RAM_BEGIN + (PALETTE_SIZE * 4) + 0; spritePaletteTblEntryZero <= PALETTE_RAM_END; spritePaletteTblEntryZero += PALETTE_SIZE)
    {
        if (addr == spritePaletteTblEntryZero)
            addr -= 0x10; // mirror 0x3f10, 0x3f14, 0x3f18, 0x3f1c
    }

    return addr;
}

// https://www.nesdev.org/wiki/Mirroring#Nametable_Mirroring
const size_t PPU::GetNametableIndex(WORD addr) const
{
    if (addr >= NAMETABLES_MIRRORED_END) // addr < NAMETABLES_BEGIN || 
    {
        LOG_WARN("Attempted to nametable ram mirror \"Nametable RAM\" address 0x" << std::hex << addr << std::dec);
        return {};
    }

    addr = Bus::MirrorAddress(addr, NAMETABLES_TOTAL_SIZE); // mirror to 0x0000 - 0x0FFF
    
    size_t indx = 0;
    switch (*m_pMirrorType)
    {
        case MirrorType::Vertical:
            if ((addr >= 0x400 && addr <= 0x800) || (addr >= 0xC00 && addr <= 0x1000))
                indx = 1; // access second nametable
            break;
        case MirrorType::Horizontal:
            indx = (addr >= 0x800); // 0 if 0x7ff or lower, 1 otherwise
            break;

        default:
            LOG_WARN("Unimplemented nametable mirror type: " << *m_pMirrorType);
            break;
    }

    return indx;
}