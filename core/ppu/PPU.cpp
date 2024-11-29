#include "PPU.h"


PPU::PPU(std::vector<BYTE>* _pCHR_ROM) : pCHR_ROM(_pCHR_ROM), registers(&internal_registers) 
{
	m_PPURAM.assign(2 * KB, 0);
    m_OAM.assign(BYTE_MAX + 1, 0); // https://www.nesdev.org/wiki/PPU_memory_map#OAM
    m_PaletteRAM.assign(PALETTE_RAM_TOTAL_SIZE, 0);
}

BYTE PPU::ReadPPUByte()
{
    BYTE data = 0x0;
    WORD addr = GetMirroredPPUADDRAddress(true);

    if (addr < PATTERN_TABLES_END) // pattern table 0x0 - 0x1FFF
    {
        PPUDATA* ppuDataRegister = dynamic_cast<PPUDATA*>(registers.ppudata.get());

        data = ppuDataRegister->ReadBuffer();
        ppuDataRegister->WriteBuffer(pCHR_ROM->at(addr));
    }
    else if (addr < NAMETABLES_END) // nametable 0x2000 - 0x2FFF
    { // TODO:
        std::cerr << "ERROR: Reading from PPU nametable is unimplemented!" << std::endl;
    }
    else if (addr < UNUSED_END) // unused 0x3000 - 0x3EFF
        std::cerr << "ERROR: Attempted to read unusable PPU address space: 0x" << std::hex << addr << std::dec << std::endl;
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
    { // TODO:
        std::cerr << "ERROR: Writing to PPU pattern table is unimplemented!" << std::endl;
    }
    else if (addr < NAMETABLES_END) // nametable 0x2000 - 0x2FFF
    { // TODO:
        std::cerr << "ERROR: Writing to PPU nametable is unimplemented!" << std::endl;
    }
    else if (addr < UNUSED_END) // unused 0x3000 - 0x3EFF
        std::cerr << "ERROR: Attempted to write to unusable PPU address space: 0x" << std::hex << addr << std::dec << std::endl;
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

    if (addr > PALETTE_RAM_END) // 0x3F20
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