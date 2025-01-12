#pragma once

#include "../memory/Bus.h"


class CPU;

class Disassembler
{
	std::shared_ptr<Bus> m_Bus{};

public:
    Disassembler(std::shared_ptr<Bus> bus);

    struct DisasmInfo
    {
        std::string instruction{};
        size_t size{};
        std::string addrMode{};
    };

    DisasmInfo Disassemble(WORD addr);
};