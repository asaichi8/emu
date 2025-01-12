#include "Disassembler.h"
#include "CPU.h"


Disassembler::Disassembler(std::shared_ptr<Bus> bus) : m_Bus(bus)
{

}


std::string Disassembler::Disassemble(WORD addr)
{
	//WORD startAddr = addr;

	// grab instruction
	BYTE opcode = m_Bus->ReadByte(addr++);

	// look up instruction in instruction table
	const CPU::Instruction& instruction = CPU::GetInstructionTable()[opcode];
	
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
		auto val = m_Bus->ReadWord(addr); addr += 2;
		ss << std::hex << std::setw(4) << std::setfill('0') << (int)val;
		strAsm += " $" + ss.str();
	}
	else if (mode == "ABX")
	{
		auto val = m_Bus->ReadWord(addr); addr += 2;
		ss << std::hex << std::setw(4) << std::setfill('0') << (int)val;
		strAsm += " $" + ss.str() + ", X";
	}
	else if (mode == "ABY")
	{
		auto val = m_Bus->ReadWord(addr); addr += 2;
		ss << std::hex << std::setw(4) << std::setfill('0') << (int)val;
		strAsm += " $" + ss.str() + ", Y";
	}
	else if (mode == "IND")
	{
		auto val = m_Bus->ReadWord(addr); addr += 2;
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