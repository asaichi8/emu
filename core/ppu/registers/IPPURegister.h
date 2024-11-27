#pragma once
#include "typedefs.h"

class IPPURegister
{
protected:
    WORD m_regAddr;

public:
    IPPURegister(WORD regAddr) : m_regAddr(regAddr) {}
    virtual ~IPPURegister() {}

    // https://www.nesdev.org/wiki/PPU_registers#Summary
    // ensure it's an enum class rather than an enum to ensure that e.g. PPUCTRL refers to the class, and Registers::PPUCTRL refers to the reg address
    enum class Registers : WORD
    {
        PPUCTRL 	= 0x2000,	
        PPUMASK 	= 0x2001,	
        PPUSTATUS 	= 0x2002,	
        OAMADDR 	= 0x2003,	
        OAMDATA 	= 0x2004,	
        PPUSCROLL 	= 0x2005,	
        PPUADDR 	= 0x2006,	
        PPUDATA 	= 0x2007,	
        OAMDMA 		= 0x4014	
    };

    bool IsReadable() const
    {
        return (m_regAddr == (WORD)Registers::PPUSTATUS || m_regAddr == (WORD)Registers::OAMDATA || m_regAddr == (WORD)Registers::PPUDATA);
    }

    bool IsWriteable() const
    {
        return m_regAddr != (WORD)Registers::PPUSTATUS;
    }

    bool operator==(WORD r) const
    {
        return m_regAddr == r;
    }
};

class PPUCTRL : public IPPURegister
{
public:
    PPUCTRL() : IPPURegister((WORD)Registers::PPUCTRL) {}
    ~PPUCTRL() override {}
};