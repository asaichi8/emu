#include "Bus.h"


Bus::Bus(ROM* rom) : m_ROM(rom)
{
	m_PPU = std::make_unique<PPU>(&(m_ROM->CHR_ROM), &(m_ROM->mirrorType));
	m_CPURAM.assign(8 * KB, 0); // the CPU's RAM is actually only 2KB in size - the rest of the 6KB are mirrored.
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


void Bus::Reset()
{
	std::fill(m_CPURAM.begin(), m_CPURAM.end(), 0);
}

void Bus::Clock(DWORD nCycles)
{
	m_nCPUCycles += nCycles;
	m_PPU->Clock(nCycles * 3);
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
		return ReadPPURegister((PPURegAddr)(MirrorAddress(addr, PPU_REGISTER_SIZE, MIRRORED_INTERNAL_RAM_END))); // clamp to 0x2000 - 0x2007
	else if (addr == 0x4016) // TODO: get rid of magic number
		return m_Joypads[0].CPURead().to_ulong();
	else if (addr == 0x4017)
		return m_Joypads[1].CPURead().to_ulong();
	else if (addr >= PRG_RAM_START && addr <= PRG_RAM_END)
		return ReadPRGByte(addr);
	
	//return m_CPURAM[addr];
	return 0;
}

void Bus::WriteByte(WORD addr, BYTE val)
{
	if (addr < MIRRORED_INTERNAL_RAM_END)
		m_CPURAM[MirrorAddress(addr, INTERNAL_RAM_SIZE)] = val;
	else if (addr < MIRRORED_PPU_REGISTER_END)
		WritePPURegister((PPURegAddr)(MirrorAddress(addr, PPU_REGISTER_SIZE, MIRRORED_INTERNAL_RAM_END)), val);
	else if (addr == (WORD)PPURegAddr::OAMDMA) // 0x4014
		WritePPURegister(PPURegAddr::OAMDMA, val);
	else if (addr >= PRG_RAM_START && addr <= PRG_RAM_END)
		std::cerr << "Attempted to write to cartridge ROM at address 0x" << std::hex << addr << std::dec << " (this should never occur)" << std::endl;
	else if (addr == 0x4016) // TODO: get rid of magic number
	{
		m_Joypads[0].CPUWrite(val);
		m_Joypads[1].CPUWrite(val);
	}
	else if (addr >= 0x4000 && addr < 0x4020) {} // TODO: RESERVED for APU stuff
	else
		std::cerr << "Attempted to write byte 0x" << std::hex << (int)val << " at address 0x" << std::hex << addr << std::dec << " outside of CPU/PPU (not implemented)" << std::endl;
}

// TODO: what if addr = INTERNAL_RAM_SIZE, or PRG_RAM_END?
// TODO: these read/write words work, but are only for CPU internal emulation usage, so refactor keeping this in mind
WORD Bus::ReadWord(WORD addr, bool shouldWrapPage)
{
	if (addr == INTERNAL_RAM_SIZE || addr == PRG_RAM_END)
		std::cerr << "out of bounds" << std::endl;

	if (addr < MIRRORED_INTERNAL_RAM_END)
		addr = MirrorAddress(addr, INTERNAL_RAM_SIZE);
	else if (addr < MIRRORED_PPU_REGISTER_END)
	{
		std::cout << "err" << std::endl;
		// TODO: it is implemented
		//std::cerr << "Attempted to read word from PPU (not implemented)" << std::endl;
		//return 0;
	}
	else if (addr < PRG_RAM_START)
		return 0;
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

void Bus::UpdateJoypad(size_t joypad, Joypad::Button button, bool isDown)
{
	if (joypad > 1)
		throw std::out_of_range("Joypad number out of range");
		
	m_Joypads[joypad].Update(button, isDown);
}


BYTE Bus::ReadPPURegister(PPURegAddr PPUreg)
{
	switch (PPUreg)
	{
		case PPURegAddr::PPUSTATUS: return (BYTE)(m_PPU->registers.ppustatus->Read().to_ulong());
		case PPURegAddr::OAMDATA: 	return m_PPU->GetOAM().at(m_PPU->registers.oamaddr->Read().to_ulong());
		case PPURegAddr::PPUDATA: 	return m_PPU->ReadPPUByte();
		default:
			std::cerr << "ERROR: Not a readable PPU register!" << std::endl;
			return 0;
	}
}

void Bus::WritePPURegister(PPURegAddr PPUreg, BYTE val)
{
	switch (PPUreg)
	{
		case PPURegAddr::PPUCTRL: 	m_PPU->PPUCtrlWriteW(val); 													break;
		case PPURegAddr::PPUMASK: 	m_PPU->registers.ppumask->Write(val); 										break;
		case PPURegAddr::OAMADDR: 	m_PPU->registers.oamaddr->Write(val); 										break;
		case PPURegAddr::OAMDATA: 	m_PPU->WriteOAM(m_PPU->registers.oamaddr->Read().to_ulong(), val);		 	break;
		case PPURegAddr::PPUSCROLL: m_PPU->registers.ppuscroll->Write(val);										break;
		case PPURegAddr::PPUADDR: 	m_PPU->registers.ppuaddr->Write(val);										break;
		case PPURegAddr::PPUDATA: 	m_PPU->WritePPUByte(val); 													break;
		case PPURegAddr::OAMDMA: // TODO: package this into a function
		{
			std::vector<BYTE> buf(PAGE, 0);
			WORD pageNum = val << 8;
			for (int i = 0; i < PAGE; ++i)
				buf.at(i) = ReadByte(pageNum + i);

			for (const auto& data : buf)
			{
				m_PPU->WriteOAM(m_PPU->registers.oamaddr->Read().to_ulong(), data);
				dynamic_cast<OAMADDR*>(m_PPU->registers.oamaddr.get())->Increment();
			}
			break;
		}
			
		default:
			std::cerr << "ERROR: Not a writeable PPU register!" << std::endl;
			break;
	}
}

// addr must be between 0x8000 - 0xFFFF
BYTE Bus::ReadPRGByte(WORD addr)
{
	if (addr < PRG_RAM_START)
	{
		std::cerr << "ERROR: reading byte below PRG_RAM_START! (this should never occur)" << std::endl;
		return 0;
	}
	
	// mirror address if it's outside the PRG ROM size range
	if (m_ROM->PRG_ROM.size() == (16 * KB) && addr >= (PRG_RAM_START + 16 * KB))
		addr = MirrorAddress(addr, m_ROM->PRG_ROM.size(), PRG_RAM_START);

	addr -= PRG_RAM_START;
	// if (addr == 0x11D9) // SXIOPO (infinite lives in super mario)
	// 	m_ROM->PRG_ROM[addr] = 0xAD;
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

// we need a wrapper for this function because it isn't read only, and therefore cannot be const & called via GetPPU()
bool Bus::IsNMIInterruptQueuedW() { return m_PPU->IsNMIInterruptQueued(); }