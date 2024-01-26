#include <cstring>
#include <iostream>

#include "RAM.h"


RAM::RAM(size_t size) : m_RAM_Size(size)
{
    m_RAM = std::make_unique<BYTE[]>(size);
}

RAM::~RAM()
{

}


void RAM::Reset()
{
    memset(m_RAM.get(), 0, m_RAM_Size * sizeof(BYTE));
}

BYTE RAM::ReadByte(WORD addr)
{
    return m_RAM[addr];
}

WORD RAM::ReadWord(WORD addr, bool shouldWrapPage)
{
    if (addr == UINT16_MAX)
        return 0;

    // little endian
    BYTE low = m_RAM[addr];

    BYTE high{};
    if (shouldWrapPage)
        high = m_RAM[(addr & 0xFF00) | ((addr + 1) & 0x00FF)];
    else
        high = m_RAM[addr + 1];

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