#pragma once

#include "IPPURegister.h"


// $2007
class PPUDATA : public IPPURegister
{
    BYTE readBuffer = 0; // https://www.nesdev.org/wiki/PPU_registers#The_PPUDATA_read_buffer

public:
    PPUDATA(InternalRegisters* _internal_registers) : IPPURegister((WORD)PPURegAddr::PPUDATA, _internal_registers) {}
    ~PPUDATA() override {}
    
    void Write(std::bitset<8> val) override { readBuffer = val.to_ulong(); }
    std::bitset<8> Read() override { return readBuffer; }
};