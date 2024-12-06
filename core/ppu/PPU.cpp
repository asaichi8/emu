#include "PPU.h"


PPU::PPU(std::vector<BYTE>* _pCHR_ROM, MirrorType* _pMirrorType) : m_pCHR_ROM(_pCHR_ROM), m_pMirrorType(_pMirrorType), registers(&internal_registers) 
{
    for (auto& pTable : m_NametableRAM)
        pTable.assign(NAMETABLE_SIZE, 0);
    // for (auto& pTable : TEST_NameTableRAMIsRealZero)
    //     pTable.assign(NAMETABLE_SIZE, 0);
    m_OAM.assign(BYTE_MAX + 1, 0); // https://www.nesdev.org/wiki/PPU_memory_map#OAM
    m_PaletteRAM.assign(PALETTE_RAM_TOTAL_SIZE, 0);
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
    
    m_nPPUCycles -= SCANLINE_END;
    m_nScanlines++;

    PPUSTATUS* ppuStatusRegister = dynamic_cast<PPUSTATUS*>(registers.ppustatus.get());
    if (m_nScanlines == VBLANK_START)
    {
        ppuStatusRegister->SetVBLANK(1);
        ppuStatusRegister->SetSprite0Hit(0);

        PPUCTRL* ppuCtrlRegister = dynamic_cast<PPUCTRL*>(registers.ppuctrl.get());
        if (ppuCtrlRegister->GetVBlankNMI())
            m_bShouldNMIInterrupt = true;
    }
    else if (m_nScanlines > VBLANK_END)
    {
        // frame complete
        ppuStatusRegister->SetSprite0Hit(0);
        m_nScanlines = -1;

        return true;
    }

    return false;
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

        data = ppuDataRegister->ReadBuffer();
        ppuDataRegister->WriteBuffer(m_pCHR_ROM->at(addr));
    }
    else if (addr < NAMETABLES_MIRRORED_END) // nametable 0x2000 - 0x3EFF
    { // TODO:
        PPUDATA* ppuDataRegister = dynamic_cast<PPUDATA*>(registers.ppudata.get());

        data = ppuDataRegister->ReadBuffer();
        auto indexes = GetNametableRAMIndx(addr);
        ppuDataRegister->WriteBuffer(m_NametableRAM[indexes.first][indexes.second]);
        // if (m_NametableRAM[indexes.first][indexes.second] == 0x0 && TEST_NameTableRAMIsRealZero[indexes.first][indexes.second] == false)
        //     std::cerr << "Attempting to read from bad memory!!" << std::endl; //brk here
    }
    else // palette ram 0x3F00 - 0x3FFF
    {
        addr = MirrorPaletteRAMAddress(addr);
        data = m_PaletteRAM.at(addr - PALETTE_RAM_BEGIN);
    }

    return data;
}

void PPU::WritePPUByte(BYTE val)
{
    WORD addr = GetMirroredPPUADDRAddress(true);

    if (addr < PATTERN_TABLES_END) // pattern table 0x0 - 0x1FFF
    {
        std::cerr << "ERROR: Can't write to CHR_ROM in PPU pattern table!" << std::endl;
    }
    else if (addr < NAMETABLES_MIRRORED_END) // nametable 0x2000 - 0x3EFF
    {
        auto indexes = GetNametableRAMIndx(addr);
        m_NametableRAM[indexes.first][indexes.second] = val;
        // if (val == 0x0)
        //     TEST_NameTableRAMIsRealZero[indexes.first][indexes.second] = true;
        // else
        //     TEST_NameTableRAMIsRealZero[indexes.first][indexes.second] = false;
    }
    else // palette ram 0x3F00 - 0x3FFF
    {
        addr = MirrorPaletteRAMAddress(addr);
        m_PaletteRAM.at(addr - PALETTE_RAM_BEGIN) = val;
    }
}


WORD PPU::GetMirroredPPUADDRAddress(bool shouldIncrement)
{
    // get address & mirror
    PPUADDR* ppuAddrRegister = dynamic_cast<PPUADDR*>(registers.ppuaddr.get());
    WORD addr = ppuAddrRegister->GetAddress();
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
        std::cerr << "WARNING: Attempted to palette ram mirror \"Palette RAM\" address 0x" << std::hex << addr << std::dec << std::endl;
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
std::pair<size_t, size_t> PPU::GetNametableRAMIndx(WORD addr)
{
    if (addr < NAMETABLES_BEGIN || addr >= NAMETABLES_MIRRORED_END)
    {
        std::cerr << "WARNING: Attempted to nametable ram mirror \"Nametable RAM\" address 0x" << std::hex << addr << std::dec << std::endl;
        return {};
    }

    addr -= NAMETABLES_TOTAL_SIZE; // normalise address to 0x0 - 1EFF
    addr = Bus::MirrorAddress(addr, NAMETABLES_TOTAL_SIZE); // mirror to 0x0000 - 0x0FFF

    size_t indx = 0;
    switch (*m_pMirrorType)
    {
        case MirrorType::Vertical:
            // To determine which nametable we need, we check the 11th bit. This tells equates to 1 if addr is between 0x400 and 0x800,
            // or between 0xC00 and 0x1000 - otherwise it's 0.
            indx = (addr & 0x0400) >> 11;
            return {indx, Bus::MirrorAddress(addr, NAMETABLE_SIZE)};
        case MirrorType::Horizontal:
            indx = (addr >= 0x800); // 0 if 0x7ff or lower, 1 otherwise
            return {indx, Bus::MirrorAddress(addr, NAMETABLE_SIZE)};

        default:
            std::cerr << "Unimplemented nametable mirror type: " << *m_pMirrorType << std::endl;
            return {};
    }
}