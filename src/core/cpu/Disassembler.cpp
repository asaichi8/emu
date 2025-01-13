#include "Disassembler.h"
#include "CPU.h"


Disassembler::Disassembler(std::shared_ptr<Bus> bus) : m_Bus(bus)
{

}


Disassembler::DisasmInfo Disassembler::Disassemble(WORD addr)
{
    DisasmInfo info;

	//WORD startAddr = addr;

	// grab instruction
	BYTE opcode = m_Bus->ReadByte(addr++);

	// look up instruction in instruction table
	const CPU::Instruction& instruction = CPU::GetInstructionTable()[opcode];
	
	// get name of instruction
	info.instruction = instruction.strName;
	info.addrMode = instruction.strAddrMode;
    info.size = 1;

    // set up stringstream used to convert vals to hex
	std::stringstream ss;
    ss << std::hex << std::setfill('0') << std::uppercase;

    // https://www.nesdev.org/obelisk-6502-guide/addressing.html
	if (info.addrMode == "IMP")
	{
		//
	}
	else if (info.addrMode == "ACC")
	{
		info.instruction += " A";
	}
	else if (info.addrMode == "IMM")
	{
		auto val = m_Bus->ReadByte(addr);
        info.size++;
		ss << std::setw(2) << (int)val;
		info.instruction += " #$" + ss.str();
	}
	else if (info.addrMode == "ABS")
	{
		auto val = m_Bus->ReadWord(addr);
        info.size += 2;
		ss << std::setw(4) << (int)val;
		info.instruction += " $" + ss.str();
	}
	else if (info.addrMode == "ABX")
	{
		auto val = m_Bus->ReadWord(addr);
        info.size += 2;
		ss << std::setw(4) << (int)val;
		info.instruction += " $" + ss.str() + ", X";
	}
	else if (info.addrMode == "ABY")
	{
		auto val = m_Bus->ReadWord(addr);
        info.size += 2;
		ss << std::setw(4) << (int)val;
		info.instruction += " $" + ss.str() + ", Y";
	}
	else if (info.addrMode == "IND")
	{
		auto val = m_Bus->ReadWord(addr);
        info.size += 2;
		ss << std::setw(4) << (int)val;
		info.instruction += " ($" + ss.str() + ")";
	}
	else if (info.addrMode == "REL")
	{
		auto val = m_Bus->ReadByte(addr);
        info.size++;
		WORD branchAddr = addr + val; // TODO: check for overflow?
		ss << std::setw(4) << (int)branchAddr;
		info.instruction += " $" + ss.str();
	}
	else if (info.addrMode == "ZPG")
	{
		auto val = m_Bus->ReadByte(addr);
        info.size++;
		ss << std::setw(2) << (int)val;
		info.instruction += " $00" + ss.str();
	}
	else if (info.addrMode == "ZPX")
	{
		auto val = m_Bus->ReadByte(addr);
        info.size++;
		ss << std::setw(2) << (int)val;
		info.instruction += " $00" + ss.str() + ", X";
	}
	else if (info.addrMode == "ZPY")
	{
		auto val = m_Bus->ReadByte(addr);
        info.size++;
		ss << std::setw(2) << (int)val;
		info.instruction += " $00" + ss.str() + ", Y";
	}
	else if (info.addrMode == "IZY")
	{
		auto val = m_Bus->ReadByte(addr);
        info.size++;
		ss << std::setw(2) << (int)val;
		info.instruction += " ($" + ss.str() + "), Y";
	}
	else if (info.addrMode == "IZX")
	{
		auto val = m_Bus->ReadByte(addr);
        info.size++;
		ss << std::setw(2) << (int)val;
		info.instruction += " ($" + ss.str() + ", X)";
	}

	return info;
}