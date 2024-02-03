#pragma once

#include <vector>
#include <memory>
#include "typedefs.h"

// https://www.nesdev.org/wiki/CPU_memory_map
#define INTERNAL_RAM_SIZE 0x800
#define MIRRORED_INTERNAL_RAM_END 0x2000
#define PPU_REGISTER_SIZE 0x08
#define MIRRORED_PPU_REGISTER_END 0x4000


class RAM
{

public:
    virtual ~RAM() {}

    virtual BYTE ReadByte(WORD addr) = 0;
    virtual WORD ReadWord(WORD addr, bool shouldWrapPage = false) = 0;
    virtual void WriteByte(WORD addr, BYTE val) = 0;
    virtual void WriteWord(WORD addr, WORD val) = 0;
};

class Bus : public RAM
{
    std::vector<BYTE> m_RAM{};

public:
    Bus()
    {
        m_RAM.resize(64 * 1024);
        Reset();
    }

    BYTE ReadByte(WORD addr)
    {
        if (addr < MIRRORED_INTERNAL_RAM_END)
            return m_RAM[addr % INTERNAL_RAM_SIZE];
        else if (addr < MIRRORED_PPU_REGISTER_END)
            return m_RAM[addr % PPU_REGISTER_SIZE];
        
        return m_RAM[addr];
    }


    WORD ReadWord(WORD addr, bool shouldWrapPage = false)
    {
        if (addr < MIRRORED_INTERNAL_RAM_END)
            addr = addr % INTERNAL_RAM_SIZE;
        else if (addr < MIRRORED_PPU_REGISTER_END)
            addr = addr % PPU_REGISTER_SIZE;

        BYTE low = m_RAM[addr];

        BYTE high{};
        if (shouldWrapPage)
            high = m_RAM[(addr & 0xFF00) | ((addr + 1) & 0x00FF)];
        else
            high = m_RAM[addr + 1];

        return (WORD(high) << 8) + low;
    }

    void WriteByte(WORD addr, BYTE val)
    {
        if (addr < MIRRORED_INTERNAL_RAM_END)
            m_RAM[addr % INTERNAL_RAM_SIZE] = val;
        else if (addr < MIRRORED_PPU_REGISTER_END)
            m_RAM[addr % PPU_REGISTER_SIZE] = val;
            
        m_RAM[addr] = val;
    }

    void WriteWord(WORD addr, WORD val)
    {
        WORD effectiveAddr = addr;

        if (addr < MIRRORED_INTERNAL_RAM_END)
            effectiveAddr = addr % INTERNAL_RAM_SIZE;
        else if (addr < MIRRORED_PPU_REGISTER_END)
            effectiveAddr = addr % PPU_REGISTER_SIZE;

        m_RAM[effectiveAddr] = BYTE(val & 0x00FF); // set low byte
        m_RAM[effectiveAddr + 1] = BYTE((val >> 8) & 0xFF); // set high byte
    }

    void Reset()
    {
        std::fill(m_RAM.begin(), m_RAM.end(), 0);
    }
};