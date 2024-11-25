#include "Bus.h"


Bus::Bus(ROM* rom) : m_ROM(rom)
{
	m_RAM.assign(8 * KB, 0);
}


void Bus::Reset()
{
	std::fill(m_RAM.begin(), m_RAM.end(), 0);
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

BYTE Bus::ReadByte(WORD addr)
{
	// On the NES, only 2KiB of RAM is accessible due to hardware limitations. The CPU accesses RAM through addresses 0x0000 to 0x1FFF, however, which
	// is an 8KiB range. As a result, any time the CPU attempts to access an address past 2KiB (0x800), the address is mirrored back to an address
	// within the initial 2KiB.
	if (addr < MIRRORED_INTERNAL_RAM_END)
		return m_RAM[addr % INTERNAL_RAM_SIZE]; // return mirrored address
	else if (addr >= PRG_RAM_START && addr <= PRG_RAM_END)
		return ReadPRGByte(addr);
	else if (addr < MIRRORED_PPU_REGISTER_END)
	{
		//return m_RAM[addr % PPU_REGISTER_SIZE];
		std::cerr << "Attempted to read byte from PPU (not implemented)" << std::endl;
		return 0;
	}
	
	return m_RAM[addr];
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

	BYTE low = m_RAM[addr];

	BYTE high{};
	if (shouldWrapPage)
		high = m_RAM[(addr & 0xFF00) | ((addr + 1) & 0x00FF)];
	else
		high = m_RAM[addr + 1];

	return (WORD(high) << 8) + low;
}

void Bus::WriteByte(WORD addr, BYTE val)
{
	if (addr < MIRRORED_INTERNAL_RAM_END)
		m_RAM[addr % INTERNAL_RAM_SIZE] = val;
	else
	{
		std::cerr << "Attempted to write byte outside of CPU (not implemented)" << std::endl;
	}
	/*else if (addr < MIRRORED_PPU_REGISTER_END)
		m_RAM[addr % PPU_REGISTER_SIZE] = val;
	else
		m_RAM[addr] = val;*/
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
	//else if (addr < MIRRORED_PPU_REGISTER_END)
	//   addr %= PPU_REGISTER_SIZE;

	m_RAM[addr] = BYTE(val & 0x00FF); // set low byte
	// ensure addr is correctly wrapped around with % INTERNAL_RAM_SIZE
	m_RAM[(addr + 1) % INTERNAL_RAM_SIZE] = BYTE((val >> 8) & 0xFF); // set high byte
}