#pragma once

class PPURegisters
{
public:
    PPURegisters();

    // https://www.nesdev.org/wiki/PPU_registers#Summary
    enum Registers
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

    static bool IsReadable(Registers reg);
    static bool IsWritable(Registers reg);
};
