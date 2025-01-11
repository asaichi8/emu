#pragma once

#include <bitset>
#include "pch.h"


class IPPURegister
{
    WORD m_regAddr;

public:
	// https://www.nesdev.org/wiki/PPU_registers#Internal_registers
	struct InternalRegisters {
        // TODO: use v and t
		WORD v = 0;	// "Current VRAM address (15 bits)"
		WORD t = 0; // "Temporary VRAM address (15 bits)" : address of top-left onscreen tile
		BYTE x = 0; // "Fine X scroll (3 bits)"
		// Toggles on PPUSCROLL/PPUADDR Write, clears on PPUSTATUS read : https://www.nesdev.org/wiki/PPU_registers#Internal_registers
		bool w = 0; // Determines whether we're writing the high/low bit (PPUADDR/PPUDATA registers)
	};

protected:
    InternalRegisters* internal_registers; // all registers point to one InternalRegisters class

public:
    IPPURegister(WORD regAddr, InternalRegisters* _internal_registers) : m_regAddr(regAddr), internal_registers(_internal_registers) {}
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

    //virtual std::bitset<8>* GetRegVal() { return nullptr; }
    virtual void Write(std::bitset<8> val) { std::cerr << "ERROR: Unimplemented PPU register write!" << std::endl; }
    virtual std::bitset<8> Read()          { std::cerr << "ERROR: Unimplemented PPU register read!"  << std::endl; return 0; }

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