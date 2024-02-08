#include "Bus.h"


Bus::Bus(ROM* rom) : m_ROM(rom)
{
    m_RAM.assign(8 * KB, 0);
}


void Bus::Reset()
{
    std::fill(m_RAM.begin(), m_RAM.end(), 0);
}

BYTE Bus::ReadPRGByte(WORD addr)
{
    addr -= 32 * KB;
    
    if (m_ROM->PRG_ROM.size() == (16 * KB) && addr >= (16 * KB))
        addr %= 16 * KB;
    
    return m_ROM->PRG_ROM[addr];
}

WORD Bus::ReadPRGWord(WORD addr, bool shouldWrapPage)
{    
    BYTE low = ReadPRGByte(addr);
    BYTE high{};
    if (shouldWrapPage)
        high = ReadPRGByte((addr & 0xFF00) | ((addr + 1) & 0x00FF));
    else
        high = ReadPRGByte(addr + 1);
        
    return (WORD(high) << 8) + low;
}

BYTE Bus::ReadByte(WORD addr)
{
    if (addr < MIRRORED_INTERNAL_RAM_END)
        return m_RAM[addr % INTERNAL_RAM_SIZE];
    else if (addr >= PRG_RAM_START && addr <= PRG_RAM_END)
        return ReadPRGByte(addr);
    else if (addr < MIRRORED_PPU_REGISTER_END)
    {
        //return m_RAM[addr % PPU_REGISTER_SIZE];
        std::cerr << "Attempted to read byte from PPU (not implemented)" << std::endl;
        return 0;
    }
    
    return m_RAM[addr];
}

WORD Bus::ReadWord(WORD addr, bool shouldWrapPage)
{
    if (addr < MIRRORED_INTERNAL_RAM_END)
        addr %= INTERNAL_RAM_SIZE;
    else if (addr < MIRRORED_PPU_REGISTER_END)
    {
        //addr %= PPU_REGISTER_SIZE;
        std::cerr << "Attempted to read word from PPU (not implemented)" << std::endl;
        return 0;
    }
    else if (addr >= PRG_RAM_START && addr <= PRG_RAM_END)
        return ReadPRGWord(addr, shouldWrapPage);

    BYTE low = m_RAM[addr];

    BYTE high{};
    if (shouldWrapPage)
        high = m_RAM[(addr & 0xFF00) | ((addr + 1) & 0x00FF)];
    else
        high = m_RAM[addr + 1];

    return (WORD(high) << 8) + low;
}

void Bus::WriteByte(WORD addr, BYTE val)
{
    if (addr < MIRRORED_INTERNAL_RAM_END)
        m_RAM[addr % INTERNAL_RAM_SIZE] = val;
    else
    {
        std::cerr << "Attempted to write byte outside of CPU (not implemented)" << std::endl;
    }
    /*else if (addr < MIRRORED_PPU_REGISTER_END)
        m_RAM[addr % PPU_REGISTER_SIZE] = val;
    else
        m_RAM[addr] = val;*/
}

void Bus::WriteWord(WORD addr, WORD val)
{
    if (addr < MIRRORED_INTERNAL_RAM_END)
        addr %= INTERNAL_RAM_SIZE;
    else
    {
        std::cerr << "Attempted to write word outside of CPU (not implemented)" << std::endl;
        return;
    }
    //else if (addr < MIRRORED_PPU_REGISTER_END)
    //   addr %= PPU_REGISTER_SIZE;

    m_RAM[addr] = BYTE(val & 0x00FF); // set low byte
    m_RAM[addr + 1] = BYTE((val >> 8) & 0xFF); // set high byte
}