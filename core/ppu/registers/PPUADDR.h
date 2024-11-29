#pragma once

#include "IPPURegister.h"


// https://www.nesdev.org/wiki/PPU_registers#PPUADDR_-_VRAM_address_($2006_write)
class PPUADDR : public IPPURegister
{
    WORD ppuAddr = 0;

public:
    PPUADDR(InternalRegisters* _internal_registers) : IPPURegister((WORD)PPURegAddr::PPUADDR, _internal_registers) {}
    ~PPUADDR() override {}

    void Write(std::bitset<8> val) override 
    { 
        if (internal_registers->w) // write low byte
            ppuAddr = (ppuAddr & 0xFF00) | (WORD)val.to_ulong();
        else // write high byte
            ppuAddr = (ppuAddr & 0x00FF) | ((WORD)val.to_ulong() << 8);

        internal_registers->w = !internal_registers->w;
    }

    void IncrementAddr(BYTE increment)
    {
        if ((WORD_MAX - ppuAddr) < increment) // check for overflow
        {
            ppuAddr += increment - (WORD_MAX + 1); // handle overflow
            return;
        }

        ppuAddr += increment;
    }

    WORD GetAddress() { return ppuAddr; }
};