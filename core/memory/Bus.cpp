#include "Bus.h"


Bus::Bus(ROM* rom) : m_ROM(rom)
{
	m_CPURAM.assign(8 * KB, 0); // the CPU's RAM is actually only 2KB in size - the rest of the 6KB are mirrored.
}


void Bus::Reset()
{
	std::fill(m_CPURAM.begin(), m_CPURAM.end(), 0);
}

// https://www.nesdev.org/wiki/CPU_memory_map
BYTE Bus::ReadByte(WORD addr)
{
	// On the NES, only 2KiB of RAM is accessible due to hardware limitations. The CPU accesses RAM through addresses 0x0000 to 0x1FFF, however, which
	// is an 8KiB range. As a result, any time the CPU attempts to access an address past 2KB (0x800), the address is mirrored back to an address
	// within the initial 2KiB.
	if (addr < MIRRORED_INTERNAL_RAM_END)
		return m_CPURAM[addr % INTERNAL_RAM_SIZE]; // clamp to 0x000 - 0x7FF
	else if (addr < MIRRORED_PPU_REGISTER_END)
	{
		addr -= MIRRORED_INTERNAL_RAM_END; // normalise from 0x2000 - 0x3FFF to 0x0000 - 0x1FFF so we can mirror the address
		addr %= PPU_REGISTER_SIZE; // clamp to 0x00 - 0x07
		addr += MIRRORED_INTERNAL_RAM_END; // go back to 0x2000 - 0x2007
		
		return ReadPPUByte((PPURegisters::Registers)addr);
	}
	else if (addr >= PRG_RAM_START && addr <= PRG_RAM_END)
		return ReadPRGByte(addr);
	
	return m_CPURAM[addr];
}

// TODO: what if addr = INTERNAL_RAM_SIZE, or PRG_RAM_END?
WORD Bus::ReadWord(WORD addr, bool shouldWrapPage)
{
	if (addr < MIRRORED_INTERNAL_RAM_END)
		addr %= INTERNAL_RAM_SIZE;
	else if (addr < MIRRORED_PPU_REGISTER_END)
	{
		//addr %= PPU_REGISTER_SIZE;
		std::cerr << "Attempted to read word from PPU (not implemented)" << std::endl;
		return 0;
	}
	else if (addr >= PRG_RAM_START && addr <= PRG_RAM_END)
		return ReadPRGWord(addr, shouldWrapPage);

	BYTE low = m_CPURAM[addr];

	BYTE high{};
	if (shouldWrapPage)
		high = m_CPURAM[(addr & 0xFF00) | ((addr + 1) & 0x00FF)];
	else
		high = m_CPURAM[addr + 1];

	return (WORD(high) << 8) + low;
}

void Bus::WriteByte(WORD addr, BYTE val)
{
	if (addr < MIRRORED_INTERNAL_RAM_END)
		m_CPURAM[addr % INTERNAL_RAM_SIZE] = val;
	else if (addr < MIRRORED_PPU_REGISTER_END)
	{
		addr -= MIRRORED_INTERNAL_RAM_END; // normalise from 0x2000 - 0x3FFF to 0x0000 - 0x1FFF so we can mirror the address
		addr %= PPU_REGISTER_SIZE; // clamp to 0x00 - 0x07
		addr += MIRRORED_INTERNAL_RAM_END; // go back to 0x2000 - 0x2007
		
		WritePPUByte((PPURegisters::Registers)addr);
	}
	else if (addr == PPURegisters::Registers::OAMDMA) // 0x4014
		WritePPUByte(PPURegisters::Registers::OAMDMA);
	else
		std::cerr << "Attempted to write byte outside of CPU/PPU (not implemented)" << std::endl;
}

void Bus::WriteWord(WORD addr, WORD val)
{
	if (addr < MIRRORED_INTERNAL_RAM_END)
		addr %= INTERNAL_RAM_SIZE;
	else
	{
		std::cerr << "Attempted to write word outside of CPU (not implemented)" << std::endl;
		return;
	}

	m_CPURAM[addr] = BYTE(val & 0x00FF); // set low byte
	// ensure addr is correctly wrapped around with % INTERNAL_RAM_SIZE
	m_CPURAM[(addr + 1) % INTERNAL_RAM_SIZE] = BYTE((val >> 8) & 0xFF); // set high byte
}


/*
WORD Bus::ClampAddress(WORD addr, WORD size)
{

}
*/

BYTE Bus::ReadPPUByte(PPURegisters::Registers PPUreg)
{
	if (!PPURegisters::IsReadable(PPUreg))
	{
		std::cerr << "ERROR: Attempted to read write-only PPU register " << PPUreg << std::endl;
		return NULL;
	}

	// TODO: handle unhandled readable registers
	switch (PPUreg)
	{
		case PPURegisters::Registers::PPUSTATUS:
			std::cerr << "ERROR: Attempted to read from unimplemented PPU register PPUSTATUS" << std::endl;
			return NULL;
		case PPURegisters::Registers::OAMDATA:
			std::cerr << "ERROR: Attempted to read from unimplemented PPU register OAMDATA" << std::endl;
			return NULL;
		case PPURegisters::Registers::PPUDATA:
			std::cerr << "ERROR: Attempted to read from unimplemented PPU register PPUDATA" << std::endl;
			return NULL;
		default:
			std::cerr << "ERROR: Unhandled readable register! (this should never occur)" << std::endl;
			return NULL;
	}
}

BYTE Bus::WritePPUByte(PPURegisters::Registers PPUreg)
{
	if (!PPURegisters::IsWritable(PPUreg))
	{
		std::cerr << "ERROR: Attempted to write read-only PPU register " << PPUreg << std::endl;
		return NULL;
	}

	// TODO: handle unhandled writeable registers
	switch (PPUreg)
	{
		case PPURegisters::Registers::PPUCTRL:
			std::cerr << "ERROR: Attempted to read from unimplemented PPU register PPUCTRL" << std::endl;
			return NULL;
		case PPURegisters::Registers::PPUMASK:
			std::cerr << "ERROR: Attempted to read from unimplemented PPU register PPUMASK" << std::endl;
			return NULL;
		case PPURegisters::Registers::OAMADDR:
			std::cerr << "ERROR: Attempted to read from unimplemented PPU register OAMADDR" << std::endl;
			return NULL;
		case PPURegisters::Registers::OAMDATA:
			std::cerr << "ERROR: Attempted to read from unimplemented PPU register OAMDATA" << std::endl;
			return NULL;
		case PPURegisters::Registers::PPUSCROLL:
			std::cerr << "ERROR: Attempted to read from unimplemented PPU register PPUSCROLL" << std::endl;
			return NULL;
		case PPURegisters::Registers::PPUADDR:
			std::cerr << "ERROR: Attempted to read from unimplemented PPU register PPUADDR" << std::endl;
			return NULL;
		case PPURegisters::Registers::PPUDATA:
			std::cerr << "ERROR: Attempted to read from unimplemented PPU register PPUDATA" << std::endl;
			return NULL;
		case PPURegisters::Registers::OAMDMA:
			std::cerr << "ERROR: Attempted to read from unimplemented PPU register OAMDMA" << std::endl;
			return NULL;
		default:
			std::cerr << "ERROR: Unhandled writeable register! (this should never occur)" << std::endl;
			return NULL;
	}
}

BYTE Bus::ReadPRGByte(WORD addr)
{
	addr -= 32 * KB; // normalise address - CPU addresses it from 32KiB to 64KiB
	
	if (m_ROM->PRG_ROM.size() == (16 * KB) && addr >= (16 * KB))
		addr %= 16 * KB; // mirror address if it's outside the PRG ROM size range
	
	return m_ROM->PRG_ROM[addr];
}

WORD Bus::ReadPRGWord(WORD addr, bool shouldWrapPage)
{    
	BYTE low = ReadPRGByte(addr);
	BYTE high{};
	if (shouldWrapPage)
		high = ReadPRGByte((addr & 0xFF00) | ((addr + 1) & 0x00FF));
	else
		high = ReadPRGByte(addr + 1);
		
	return (WORD(high) << 8) + low;
}