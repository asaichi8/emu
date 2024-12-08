#pragma once

#include "IPPURegister.h"


class OAMADDR : public IPPURegister
{
    BYTE oamAddr{};

public:
    OAMADDR(InternalRegisters* _internal_registers) : IPPURegister((WORD)PPURegAddr::OAMADDR, _internal_registers) {}
    ~OAMADDR() override {}

    void Write(std::bitset<8> val) override { oamAddr = val.to_ulong(); }
    std::bitset<8> Read() override { return oamAddr; }

    void Increment() { oamAddr == BYTE_MAX ? oamAddr = 0 : oamAddr++; }
};