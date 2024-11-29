#include "PPU.h"


PPU::PPU() : registers(&internal_registers) 
{
	m_PPURAM.assign(2 * KB, 0);
    m_OAM.assign(BYTE_MAX + 1, 0); // https://www.nesdev.org/wiki/PPU_memory_map#OAM
}

BYTE PPU::ReadPPUByte()
{
    BYTE data = 0x0;
    WORD addr = GetPPUADDRAddress(true);

    if (addr < PATTERN_TABLE_TOTAL_SIZE) // pattern table 0x0 - 0x1FFF
    { // TODO:
        std::cerr << "ERROR: Reading from PPU pattern table is unimplemented!" << std::endl;
    }
    else if (addr < NAMETABLE_TOTAL_SIZE) // nametable 0x2000 - 0x2FFF
    { // TODO:
        std::cerr << "ERROR: Reading from PPU nametable is unimplemented!" << std::endl;
    }
    else if (addr < UNUSED_TOTAL_SIZE) // unused 0x3000 - 0x3EFF
        std::cerr << "ERROR: Attempted to read unusable PPU address space: 0x" << std::hex << addr << std::dec << std::endl;
    else // palette ram 0x3F00 - 0x3FFF
    {
        if (addr > PALETTE_RAM_BEGIN + PALETTE_RAM_SIZE) // if greater than 0x3F20...
            addr = Bus::MirrorAddress(addr, PALETTE_RAM_SIZE, PALETTE_RAM_BEGIN); // mirror all addresses from 0x3F20 - 0x3FFF, size 0x20

        std::cerr << "ERROR: Reading from PPU palette ram is unimplemented!" << std::endl;
    }

    return data;
}

void PPU::WritePPUByte(BYTE val)
{
    WORD addr = GetPPUADDRAddress(true);

    if (addr < PATTERN_TABLE_TOTAL_SIZE) // pattern table 0x0 - 0x1FFF
    { // TODO:
        std::cerr << "ERROR: Writing to PPU pattern table is unimplemented!" << std::endl;
    }
    else if (addr < NAMETABLE_TOTAL_SIZE) // nametable 0x2000 - 0x2FFF
    { // TODO:
        std::cerr << "ERROR: Writing to PPU nametable is unimplemented!" << std::endl;
    }
    else if (addr < UNUSED_TOTAL_SIZE) // unused 0x3000 - 0x3EFF
        std::cerr << "ERROR: Attempted to write to unusable PPU address space: 0x" << std::hex << addr << std::dec << std::endl;
    else // palette ram 0x3F00 - 0x3FFF
    {
        if (addr > PALETTE_RAM_BEGIN + PALETTE_RAM_SIZE)
            addr = Bus::MirrorAddress(addr, PALETTE_RAM_SIZE, PALETTE_RAM_BEGIN); 

        std::cerr << "ERROR: Writing to PPU palette ram is unimplemented!" << std::endl;
    }
}


WORD PPU::GetPPUADDRAddress(bool shouldIncrement)
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