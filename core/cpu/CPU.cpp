#include "CPU.h"

#include <cstring>
#include <iostream>


CPU::CPU(RAM* ram) : m_RAM(ram)
{
    this->Reset();
}


/// @brief Executes an instruction.
/// @param instruction Instruction to be executed.
void CPU::Execute(const Instruction &instruction)
{
    // first call the addressing mode, so we can get the address of what we're acting upon
    WORD operand = (this->*(instruction.addrMode))();

    // now call opcode normally with the real operand as the address
    (this->*(instruction.opcode))(operand);

}

/// @brief Resets the processor to a known state.
void CPU::Reset()
{
    reg.program_counter = 0;
    reg.accumulator = 0;
    reg.X = 0;
    reg.Y = 0;
    reg.stack_pointer = 0xFD; // https://www.nesdev.org/wiki/CPU_power_up_state#cite_note-reset-stack-push-3
    reg.status_register = 0;

    m_RAM->Reset();
}