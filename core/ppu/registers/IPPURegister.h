#pragma once

#include <bitset>
#include "typedefs.h"


class IPPURegister
{
    WORD m_regAddr;

public:
    IPPURegister(WORD regAddr) : m_regAddr(regAddr) {}
    virtual ~IPPURegister() {}
    
    // https://www.nesdev.org/wiki/PPU_registers#Summary
    // ensure it's an enum class rather than an enum to ensure that e.g. PPUCTRL refers to the class, and PPURegAddr::PPUCTRL refers to the reg address
    enum class PPURegAddr : WORD
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

    virtual std::bitset<8>* GetRegVal() { return nullptr; }

    bool operator==(WORD r) const
    {
        return m_regAddr == r;
    }
};

// ensure we only need to include IPPURegister.h to include all PPU registers
#include "PPUCTRL.h"
#include "PPUMASK.h"
#include "PPUSTATUS.h"
#include "OAMADDR.h"
#include "OAMDATA.h"
#include "PPUSCROLL.h"
#include "PPUADDR.h"
#include "PPUDATA.h"
#include "OAMDMA.h"