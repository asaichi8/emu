#include "CPU.h"


CPU::CPU(std::shared_ptr<Bus> bus) : m_Bus(bus)
{
	this->Reset();
}


size_t CPU::GetInstructionLenBytes(const Instruction& instruction)
{
	if (instruction.addrMode == &CPU::IMP || 
		instruction.addrMode == &CPU::ACC)
	{
		return 1;
	}
	else if (instruction.addrMode == &CPU::IMM || 
		instruction.addrMode == &CPU::ZPG ||
		instruction.addrMode == &CPU::REL ||
		instruction.addrMode == &CPU::ZPX ||
		instruction.addrMode == &CPU::ZPY ||
		instruction.addrMode == &CPU::IZX ||
		instruction.addrMode == &CPU::IZY)
	{
		return 2;
	}

	return 3;
}

std::string CPU::Disassemble(WORD addr)
{
	// grab instruction
	BYTE opcode = m_Bus->ReadByte(addr++);

	// look up instruction in instruction table
	const Instruction& instruction = instructions[opcode];
	
	// get name of instruction
	std::string strAsm = instruction.strName;
	const std::string& mode = instruction.strAddrMode;

	std::stringstream ss;

	// https://www.nesdev.org/obelisk-6502-guide/addressing.html
	if (mode == "IMM")
	{
		//
	}
	else if (mode == "ACC")
	{
		strAsm += " A";
	}
	else if (mode == "IMM")
	{
		auto val = m_Bus->ReadByte(addr++);
		ss << std::hex << std::setw(2) << std::setfill('0') << (int)val;
		strAsm += " #$" + ss.str();
	}
	else if (mode == "ABS")
	{
		auto val = m_Bus->ReadWord(addr++);
		ss << std::hex << std::setw(4) << std::setfill('0') << (int)val;
		strAsm += " $" + ss.str();
	}
	else if (mode == "ABX")
	{
		auto val = m_Bus->ReadWord(addr++);
		ss << std::hex << std::setw(4) << std::setfill('0') << (int)val;
		strAsm += " $" + ss.str() + ", X";
	}
	else if (mode == "ABY")
	{
		auto val = m_Bus->ReadWord(addr++);
		ss << std::hex << std::setw(4) << std::setfill('0') << (int)val;
		strAsm += " $" + ss.str() + ", Y";
	}
	else if (mode == "IND")
	{
		auto val = m_Bus->ReadWord(addr++);
		ss << std::hex << std::setw(4) << std::setfill('0') << (int)val;
		strAsm += " ($" + ss.str() + ")";
	}
	else if (mode == "REL")
	{
		auto val = m_Bus->ReadByte(addr++);
		WORD branchAddr = addr + val; // TODO: check for overflow?
		ss << std::hex << std::setw(4) << std::setfill('0') << (int)branchAddr;
		strAsm += " $" + ss.str();
	}
	else if (mode == "ZPG")
	{
		auto val = m_Bus->ReadByte(addr++);
		ss << std::hex << std::setw(2) << std::setfill('0') << (int)val;
		strAsm += " $00" + ss.str();
	}
	else if (mode == "ZPX")
	{
		auto val = m_Bus->ReadByte(addr++);
		ss << std::hex << std::setw(2) << std::setfill('0') << (int)val;
		strAsm += " $00" + ss.str() + ", X";
	}
	else if (mode == "ZPY")
	{
		auto val = m_Bus->ReadByte(addr++);
		ss << std::hex << std::setw(2) << std::setfill('0') << (int)val;
		strAsm += " $00" + ss.str() + ", Y";
	}
	else if (mode == "IZY")
	{
		auto val = m_Bus->ReadByte(addr++);
		ss << std::hex << std::setw(2) << std::setfill('0') << (int)val;
		strAsm += " ($" + ss.str() + "), Y";
	}
	else if (mode == "IZX")
	{
		auto val = m_Bus->ReadByte(addr++);
		ss << std::hex << std::setw(2) << std::setfill('0') << (int)val;
		strAsm += " ($" + ss.str() + ", X)";
	}

	return strAsm;
}

void CPU::Log()
{
	const std::string logFile = "mynestest.log";
	static std::ofstream out(logFile);

	static bool hasPrintedPath = false;
	if (!hasPrintedPath)
	{
		LOG_INFO("Writing log file to: " << std::filesystem::absolute(logFile));
		hasPrintedPath = true;
	}

	out << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << reg.program_counter << "  ";
	out << std::hex << std::setw(2) << std::setfill('0') << (int)m_curOpcode << ' ';
	for (int i = 1; i < GetInstructionLenBytes(instructions[m_curOpcode]); ++i)
	{
		out << std::hex << std::setw(2) << std::setfill('0') << (int)(m_Bus->ReadByte(reg.program_counter + i)) << ' ';
	}
	for (int i = 0; i < 3 - GetInstructionLenBytes(instructions[m_curOpcode]); ++i)
	{
		out << "   ";
	}
	out << " A:" << std::hex << std::setw(2) << std::setfill('0') << (int)reg.accumulator << ' ';
	out << "X:" << std::hex << std::setw(2) << std::setfill('0') << (int)reg.X << ' ';
	out << "Y:" << std::hex << std::setw(2) << std::setfill('0') << (int)reg.Y << ' ';
	out << "P:" << std::hex << std::setw(2) << std::setfill('0') << reg.status_register.to_ulong() << ' ';
	out << "SP:" << std::hex << std::setw(2) << std::setfill('0') << (int)reg.stack_pointer << ' ';
	out << "PPU:" << std::dec << std::setw(3) << std::setfill(' ') << m_Bus->GetPPU()->GetScanlineCount() << ',';
	out << std::setw(3) << std::setfill(' ') << m_Bus->GetPPU()->GetCycleCount() << ' ';
	out << "CYC:" << std::dec << (int)m_nCycles;
	out << std::endl;
}

/// @brief Starts running the CPU (https://en.wikipedia.org/wiki/Instruction_cycle)
void CPU::Run()
{
	m_curOpcode = m_Bus->ReadByte(reg.program_counter);

	reg.program_counter++;
	Execute(instructions[m_curOpcode]);
	m_nCycles += m_curCycles;

	// Log
	// Log();

	// if (reg.program_counter == 0xC66E)
	//    std::cout << std::endl; // breakpoint here

	if (m_Bus->IsNMIInterruptQueuedW())
		NMI();

	m_Bus->Clock(m_curCycles);
}


/// @brief Resets the processor to a known state.
void CPU::Reset()
{
	reg.program_counter = m_Bus->ReadWord(RESET_VECTOR); 
	reg.accumulator = 0;
	reg.X = 0;
	reg.Y = 0;
	reg.stack_pointer = 0xFD; // https://www.nesdev.org/wiki/CPU_power_up_state#cite_note-reset-stack-push-3

	reg.status_register = 0;
	reg.status_register.set(StatusRegisterFlags::UNUSED); // https://www.nesdev.org/wiki/Status_flags "No CPU effect; always pushed as 1"
	// https://www.nesdev.org/wiki/CPU_power_up_state#cite_note-1
	reg.status_register.set(StatusRegisterFlags::INTERRUPT_REQUEST);
	//reg.status_register.set(StatusRegisterFlags::BREAK_COMMAND);

	// https://6502.co.uk/lesson/reset
	// "This reset sequence lasts for seven clock cycles and after this, the computer will be usable. "
	m_curCycles += 7;
	m_Bus->Clock(m_nCycles);

	m_Bus->Reset();
}


// https://www.pagetable.com/?p=410
void CPU::IRQ()
{
	if (reg.status_register.test(StatusRegisterFlags::INTERRUPT_REQUEST))
		return;

	auto savedReg = reg.status_register;
	savedReg.set(StatusRegisterFlags::UNUSED);
	savedReg.reset(StatusRegisterFlags::BREAK_COMMAND);

	PushStackWord(reg.program_counter);
	PushStackByte((BYTE)(savedReg.to_ulong()));

	reg.status_register.set(StatusRegisterFlags::INTERRUPT_REQUEST);
	
	m_curCycles += 7;
	reg.program_counter = m_Bus->ReadWord(IRQ_VECTOR); //(m_Bus->ReadByte(IRQ_VECTOR + 1) << 8) | m_Bus->ReadByte(IRQ_VECTOR); 
}
	
void CPU::NMI()
{
	auto savedReg = reg.status_register;
	savedReg.set(StatusRegisterFlags::UNUSED);
	savedReg.reset(StatusRegisterFlags::BREAK_COMMAND);

	PushStackWord(reg.program_counter);
	PushStackByte((BYTE)(savedReg.to_ulong()));

	reg.status_register.set(StatusRegisterFlags::INTERRUPT_REQUEST);

	// TODO: cycles?
	m_curCycles += 7;
	reg.program_counter = m_Bus->ReadWord(NMI_VECTOR); // (m_Bus->ReadByte(NMI_VECTOR + 1) << 8) | m_Bus->ReadByte(NMI_VECTOR);
}



/// @brief Executes an instruction.
/// @param instruction Instruction to be executed.
void CPU::Execute(const Instruction& instruction)
{ 
	m_curCycles = instruction.cycles;
	m_bNeedsExtraCycle = instruction.extraCycle; // sets member variable if we need to check for extra cycle

	// first call the addressing mode, so we can get the address of what we're acting upon
	WORD operand = (this->*(instruction.addrMode))();

	// now call opcode normally with the real operand as the address
	(this->*(instruction.opcode))(operand);
}


// Opcode helper functions
/// @brief Pushes a byte to the stack.
/// @param val Byte to be pushed.
void CPU::PushStackByte(BYTE val)
{
	m_Bus->WriteByte(STACK + reg.stack_pointer, val);

	reg.stack_pointer--;
}

/// @brief Pops and retrieves a byte from the stack.
/// @return Popped byte.
BYTE CPU::PopStackByte()
{
	reg.stack_pointer++;

	return m_Bus->ReadByte(STACK + reg.stack_pointer);
}


/// @brief Pushes a word to the stack.
/// @param val Word to be pushed.
void CPU::PushStackWord(WORD val)
{
	PushStackByte((val >> 8) & 0xFF);
	PushStackByte(val & 0xFF);
}

/// @brief Pops and retrieves a word from the stack.
/// @return Popped word.
WORD CPU::PopStackWord()
{
	BYTE low = PopStackByte();
	BYTE high = PopStackByte();

	return ((high << 8) | low);
}

/// @brief Checks if two addresses are on the same page or not.
/// @param addr1 First address to be compared.
/// @param addr2 Second address to be compared.
/// @return Returns true if the two addresses are on the same page, false if otherwise.PIRAT
bool CPU::IsOnSamePage(WORD addr1, WORD addr2)
{
	return ((addr1 & 0xFF00) == (addr2 & 0xFF00));
}
