#pragma once

#include "IPPURegister.h"

// https://www.nesdev.org/wiki/PPU_registers#PPUADDR_-_VRAM_address_($2006_write)
class PPUADDR : public IPPURegister
{
    // PPU address MUST be between 0 and 0x3FFF incl.
    static const WORD PPU_ADDRESS_SPACE_END = 0x4000; // TODO: there's another variable that does the same thing
    WORD ppuAddr = 0;

public:
    PPUADDR(InternalRegisters *_internal_registers) : IPPURegister((WORD)PPURegAddr::PPUADDR, _internal_registers) {}
    ~PPUADDR() override {}

    void Write(std::bitset<8> val) override
    {
        if (internal_registers->w) // preserve high byte & write low byte //this->w
            ppuAddr = (ppuAddr & 0xFF00) | (WORD)val.to_ulong();
        else // preserve low byte & write high byte
            ppuAddr = (ppuAddr & 0x00FF) | ((WORD)val.to_ulong() << 8);

        ppuAddr %= PPU_ADDRESS_SPACE_END;

        internal_registers->w = !internal_registers->w;
    }

    void IncrementAddr(BYTE increment)
    {
        ppuAddr = (ppuAddr + increment) % PPU_ADDRESS_SPACE_END;
    }

    WORD GetAddress() { return ppuAddr; }
};