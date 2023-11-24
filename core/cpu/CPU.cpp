#include "CPU.h"

#include <cstring>

CPU::CPU(RAM* ram) : m_RAM(ram)
{
    this->reset();
}

void CPU::reset()
{
    reg.program_counter = 0;
    reg.accumulator = 0;
    reg.X = 0;
    reg.Y = 0;
    reg.stack_pointer = 0;
    reg.status_register = 0;

    memset(m_RAM->ram, 0, sizeof(m_RAM->ram));
}