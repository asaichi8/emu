#include "CPU.h"

#include <cstring>
#include <iostream>


CPU::CPU(RAM* ram) : m_RAM(ram)
{

}


/// @brief Starts running the CPU (https://en.wikipedia.org/wiki/Instruction_cycle)
void CPU::Run()
{
    this->Reset();

    while (true)
    {
        BYTE opcode = m_RAM->ReadByte(reg.program_counter);
        Execute(instructions[opcode]);
    }
}


/// @brief Resets the processor to a known state.
void CPU::Reset()
{
    // https://en.wikipedia.org/wiki/Reset_vector "The reset vector for 6502 processor family is a 16-bit address stored at 0xFFFC and 0xFFFD."
    reg.program_counter = m_RAM->ReadWord(0xFFFC); 
    reg.accumulator = 0;
    reg.X = 0;
    reg.Y = 0;
    reg.stack_pointer = 0xFD; // https://www.nesdev.org/wiki/CPU_power_up_state#cite_note-reset-stack-push-3

    reg.status_register = 0;
    reg.status_register.set(StatusRegisterFlags::UNUSED); // https://www.nesdev.org/wiki/Status_flags "No CPU effect; always pushed as 1"
    // https://www.nesdev.org/wiki/CPU_power_up_state#cite_note-1
    reg.status_register.set(StatusRegisterFlags::BREAK_COMMAND);
    reg.status_register.set(StatusRegisterFlags::UNUSED);

    // https://6502.co.uk/lesson/reset
    // "This reset sequence lasts for seven clock cycles and after this, the computer will be usable. "
    // TODO: cycles += 7;

    m_RAM->Reset();
}

/// @brief Executes an instruction.
/// @param instruction Instruction to be executed.
void CPU::Execute(const Instruction& instruction)
{
    // first call the addressing mode, so we can get the address of what we're acting upon
    WORD operand = (this->*(instruction.addrMode))();

    // now call opcode normally with the real operand as the address
    (this->*(instruction.opcode))(operand);
}
