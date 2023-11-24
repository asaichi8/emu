#include <cstring>

#include "RAM.h"


RAM::RAM()
{

}

void RAM::Reset()
{
    memset(m_RAM, 0, sizeof(m_RAM));
}

BYTE RAM::ReadByte(WORD addr)
{
    return m_RAM[addr];
}

WORD RAM::ReadWord(WORD addr)
{
    if (addr == UINT16_MAX)
        return 0;

    // little endian
    BYTE low = m_RAM[addr];
    BYTE high = m_RAM[addr + 1];

    return (WORD(high) << 8) + low;
}

void RAM::WriteByte(WORD addr, BYTE val)
{
    m_RAM[addr] = val;
}

void RAM::WriteWord(WORD addr, WORD val)
{
    if (addr == UINT16_MAX)
        return;

    m_RAM[addr] = BYTE(val & 0x00FF); // set low byte
    m_RAM[addr + 1] = BYTE((val >> 8) & 0xFF); // set high byte
}