#pragma once

#include "../memory/Bus.h"


class CPU;

class Disassembler
{
	std::shared_ptr<Bus> m_Bus{};

public:
    Disassembler(std::shared_ptr<Bus> bus);

    std::string Disassemble(WORD addr);
};