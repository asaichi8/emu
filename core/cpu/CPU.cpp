#include "CPU.h"

#include <cstring>
#include <iostream>


CPU::CPU(RAM* ram) : m_RAM(ram)
{
    this->Reset();
}


/// @brief Starts running the CPU (https://en.wikipedia.org/wiki/Instruction_cycle)
void CPU::Run()
{
    while (true)
    {
        BYTE opcode = m_RAM->ReadByte(reg.program_counter);
        reg.program_counter++;

        // TODO: temp if statement
        if (opcode == 0x02)
            break;
            
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
    reg.status_register.set(StatusRegisterFlags::INTERRUPT_REQUEST);
    reg.status_register.set(StatusRegisterFlags::BREAK_COMMAND);

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


/// @brief Pushes a byte to the stack.
/// @param val Byte to be pushed.
void CPU::PushStackByte(BYTE val)
{
    m_RAM->WriteByte(STACK_LOCATION + reg.stack_pointer, val);

    reg.stack_pointer--;
}

/// @brief Pops and retrieves a byte from the stack.
/// @return Popped byte.
BYTE CPU::PopStackByte()
{
    reg.stack_pointer++;

    return m_RAM->ReadByte(STACK_LOCATION + reg.stack_pointer);
}

/// @brief Pushes a word to the stack.
/// @param val Word to be pushed.
void CPU::PushStackWord(WORD val)
{
    m_RAM->WriteWord(STACK_LOCATION + reg.stack_pointer, val);

    reg.stack_pointer--;
}

/// @brief Pops and retrieves a word from the stack.
/// @return Popped word.
WORD CPU::PopStackWord()
{
    reg.stack_pointer++;

    return m_RAM->ReadWord(STACK_LOCATION + reg.stack_pointer);
}
