#include "Bus.h"


Bus::Bus(ROM* rom) : m_ROM(rom)
{
	m_PPU = std::make_unique<PPU>();
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
		return m_CPURAM[MirrorAddress(addr, INTERNAL_RAM_SIZE)]; // clamp to 0x000 - 0x7FF
	else if (addr < MIRRORED_PPU_REGISTER_END)
		return ReadPPURegister((PPURegisters::Registers)(MirrorAddress(addr, PPU_REGISTER_SIZE, MIRRORED_INTERNAL_RAM_END))); // clamp to 0x2000 - 0x2007
	else if (addr >= PRG_RAM_START && addr <= PRG_RAM_END)
		return ReadPRGByte(addr);
	
	return m_CPURAM[addr];
}

// TODO: what if addr = INTERNAL_RAM_SIZE, or PRG_RAM_END?
WORD Bus::ReadWord(WORD addr, bool shouldWrapPage)
{
	if (addr < MIRRORED_INTERNAL_RAM_END)
		addr = MirrorAddress(addr, INTERNAL_RAM_SIZE);
	else if (addr < MIRRORED_PPU_REGISTER_END)
	{
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
		m_CPURAM[MirrorAddress(addr, INTERNAL_RAM_SIZE)] = val;
	else if (addr < MIRRORED_PPU_REGISTER_END)
		WritePPURegister((PPURegisters::Registers)(MirrorAddress(addr, PPU_REGISTER_SIZE, MIRRORED_INTERNAL_RAM_END)));
	else if (addr == PPURegisters::Registers::OAMDMA) // 0x4014
		WritePPURegister(PPURegisters::Registers::OAMDMA);
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


// e.g. for PPU, size = PPU_REGISTER_SIZE (0x08), startAddr = MIRRORED_INTERNAL_RAM_END (0x2000)
WORD Bus::MirrorAddress(WORD addr, WORD size, WORD startAddr)
{
	if (addr >= startAddr)
		addr -= startAddr; // e.g. for PPU registers, normalise from 0x2000 - 0x3FFF to 0x0000 - 0x1FFF so we can mirror the address
	else
		addr = 0x0;

	addr %= size; // e.g. for PPU registers, clamp to 0x00 - 0x07
	addr += startAddr; // e.g. for PPU registers, go back to 0x2000 - 0x2007

	return addr;
}

BYTE Bus::ReadPPURegister(PPURegisters::Registers PPUreg)
{
	if (!PPURegisters::IsReadable(PPUreg))
	{
		std::cerr << "ERROR: Attempted to read write-only PPU register " << PPUreg << std::endl;
		return 0;
	}

	// TODO: handle unhandled readable registers
	switch (PPUreg)
	{
		case PPURegisters::Registers::PPUSTATUS:
			std::cerr << "ERROR: Attempted to read from unimplemented PPU register PPUSTATUS" << std::endl;
			return 0;
		case PPURegisters::Registers::OAMDATA:
			std::cerr << "ERROR: Attempted to read from unimplemented PPU register OAMDATA" << std::endl;
			return 0;
		case PPURegisters::Registers::PPUDATA:
			std::cerr << "ERROR: Attempted to read from unimplemented PPU register PPUDATA" << std::endl;
			return 0;
		default:
			std::cerr << "ERROR: Unhandled readable register! (this should never occur)" << std::endl;
			return 0;
	}
}

BYTE Bus::WritePPURegister(PPURegisters::Registers PPUreg)
{
	if (!PPURegisters::IsWritable(PPUreg))
	{
		std::cerr << "ERROR: Attempted to write read-only PPU register " << PPUreg << std::endl;
		return 0;
	}

	// TODO: handle unhandled writeable registers
	switch (PPUreg)
	{
		case PPURegisters::Registers::PPUCTRL:
			std::cerr << "ERROR: Attempted to read from unimplemented PPU register PPUCTRL" << std::endl;
			return 0;
		case PPURegisters::Registers::PPUMASK:
			std::cerr << "ERROR: Attempted to read from unimplemented PPU register PPUMASK" << std::endl;
			return 0;
		case PPURegisters::Registers::OAMADDR:
			std::cerr << "ERROR: Attempted to read from unimplemented PPU register OAMADDR" << std::endl;
			return 0;
		case PPURegisters::Registers::OAMDATA:
			std::cerr << "ERROR: Attempted to read from unimplemented PPU register OAMDATA" << std::endl;
			return 0;
		case PPURegisters::Registers::PPUSCROLL:
			std::cerr << "ERROR: Attempted to read from unimplemented PPU register PPUSCROLL" << std::endl;
			return 0;
		case PPURegisters::Registers::PPUADDR:
			std::cerr << "ERROR: Attempted to read from unimplemented PPU register PPUADDR" << std::endl;
			return 0;
		case PPURegisters::Registers::PPUDATA:
			std::cerr << "ERROR: Attempted to read from unimplemented PPU register PPUDATA" << std::endl;
			return 0;
		case PPURegisters::Registers::OAMDMA:
			std::cerr << "ERROR: Attempted to read from unimplemented PPU register OAMDMA" << std::endl;
			return 0;
		default:
			std::cerr << "ERROR: Unhandled writeable register! (this should never occur)" << std::endl;
			return 0;
	}
}

BYTE Bus::ReadPRGByte(WORD addr)
{
	// mirror address if it's outside the PRG ROM size range
	if (m_ROM->PRG_ROM.size() == (16 * KB) && addr >= (PRG_RAM_START + 16 * KB))
		addr = MirrorAddress(addr, m_ROM->PRG_ROM.size(), PRG_RAM_START);

	addr -= PRG_RAM_START;
	return m_ROM->PRG_ROM[addr];
}

// TODO: probably delete this that doesn't look right
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