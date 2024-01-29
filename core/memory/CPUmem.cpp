#include "CPUmem.h"

/*BYTE CPUmem::ReadByte(WORD addr)
{
    return 0;
}


WORD CPUmem::ReadWord(WORD addr, bool shouldWrapPage = false)
{
    return 0;
}*/

void CPUmem::WriteByte(WORD addr, BYTE val)
{
    if (addr < MIRRORED_INTERNAL_RAM_END)
        m_RAM[addr % INTERNAL_RAM_SIZE] = val;
    else if (addr < MIRRORED_PPU_REGISTER_END)
        m_RAM[addr % PPU_REGISTER_SIZE] = val;
    else
        RAM::WriteByte(addr, val);
}

void CPUmem::WriteWord(WORD addr, WORD val)
{
    if (addr < MIRRORED_INTERNAL_RAM_END)
    {
        m_RAM[addr % INTERNAL_RAM_SIZE] = BYTE(val & 0x00FF); // set low byte
        m_RAM[(addr % INTERNAL_RAM_SIZE) + 1] = BYTE((val >> 8) & 0xFF); // set high byte
    }
    else if (addr < MIRRORED_PPU_REGISTER_END)
    {
        m_RAM[addr % PPU_REGISTER_SIZE] = BYTE(val & 0x00FF); // set low byte
        m_RAM[(addr % PPU_REGISTER_SIZE) + 1] = BYTE((val >> 8) & 0xFF); // set high byte
    }
    else
        RAM::WriteWord(addr, val);
}
