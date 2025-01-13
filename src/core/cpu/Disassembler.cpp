#include "Disassembler.h"
#include "CPU.h"


Disassembler::Disassembler(std::shared_ptr<Bus> bus) : m_Bus(bus)
{

}


Disassembler::DisasmInfo Disassembler::Disassemble(WORD addr)
{
    DisasmInfo info;

	// grab instruction
	BYTE opcode = m_Bus->ReadByte(addr++);

	// look up instruction in instruction table
	const CPU::Instruction& instruction = CPU::GetInstructionTable()[opcode];
	
	info.instruction = instruction.strName;
	info.addrMode = instruction.strAddrMode;
    info.size = 1; // there's only one opcode, so we start the size off at 1

	// look up and call the addressing mode handler denoted by the addressing mode string
	// these functions write to the DisasmInfo object given, appending the operand string to
	// info->instruction and modifying info->size depending on the size of the operand
	addrModeHandlers.at(info.addrMode)(&info, addr);

	return info;
}


template <typename T, typename>
std::string Disassembler::ValToHexStr(T val)
{
    std::stringstream ss;
	ss << std::hex << std::setfill('0') << std::uppercase << std::setw(sizeof(T) * 2) << (int)val;

	return ss.str();
}

void Disassembler::HandleIMP(DisasmInfo* info, WORD addr)
{
	//
}

void Disassembler::HandleACC(DisasmInfo* info, WORD addr)
{
	info->instruction += " A";
}

void Disassembler::HandleIMM(DisasmInfo* info, WORD addr)
{
	BYTE val = m_Bus->ReadByte(addr);
	info->size += sizeof(val);

	info->instruction += " #$" + ValToHexStr(val);
}

void Disassembler::HandleABS(DisasmInfo* info, WORD addr)
{
	WORD val = m_Bus->ReadWord(addr);
	info->size += sizeof(val);
    
	info->instruction += " $" + ValToHexStr(val);
}

void Disassembler::HandleABX(DisasmInfo* info, WORD addr)
{
	WORD val = m_Bus->ReadWord(addr);
	info->size += sizeof(val);
    
	info->instruction += " $" + ValToHexStr(val) + ", X";
}

void Disassembler::HandleABY(DisasmInfo* info, WORD addr)
{
	WORD val = m_Bus->ReadWord(addr);
	info->size += sizeof(val);
    
	info->instruction += " $" + ValToHexStr(val) + ", Y";
}

void Disassembler::HandleIND(DisasmInfo* info, WORD addr)
{
	WORD val = m_Bus->ReadWord(addr);
	info->size += sizeof(val);
    
	info->instruction += " ($" + ValToHexStr(val) + ")";
}

void Disassembler::HandleREL(DisasmInfo* info, WORD addr)
{
	BYTE val = m_Bus->ReadByte(addr);
	info->size += sizeof(val);
	WORD branchAddr = addr + val; // TODO: check for overflow?
	
    info->instruction += " $" + ValToHexStr(branchAddr);
}

void Disassembler::HandleZPG(DisasmInfo* info, WORD addr)
{
	BYTE val = m_Bus->ReadByte(addr);
	info->size += sizeof(val);
    
	info->instruction += " $00" + ValToHexStr(val);
}

void Disassembler::HandleZPX(DisasmInfo* info, WORD addr)
{
	BYTE val = m_Bus->ReadByte(addr);
	info->size += sizeof(val);
    
	info->instruction += " $00" + ValToHexStr(val) + ", X";
}

void Disassembler::HandleZPY(DisasmInfo* info, WORD addr)
{
	BYTE val = m_Bus->ReadByte(addr);
	info->size += sizeof(val);
    
	info->instruction += " $00" + ValToHexStr(val) + ", Y";
}

void Disassembler::HandleIZY(DisasmInfo* info, WORD addr)
{
	BYTE val = m_Bus->ReadByte(addr);
	info->size += sizeof(val);
    
	info->instruction += " ($" + ValToHexStr(val) + "), Y";
}

void Disassembler::HandleIZX(DisasmInfo* info, WORD addr)
{
	BYTE val = m_Bus->ReadByte(addr);
	info->size += sizeof(val);
    
	info->instruction += " ($" + ValToHexStr(val) + ", X)";
}