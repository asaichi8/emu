#pragma once

#include <vector>
#include <memory>
#include "../loader/ROM.h"
#include "../ppu/PPURegisters.h"
#include "../../include/typedefs.h"

// https://www.nesdev.org/wiki/CPU_memory_map
#define INTERNAL_RAM_SIZE 0x800
#define MIRRORED_INTERNAL_RAM_END 0x2000
#define PPU_REGISTER_SIZE 0x08
#define MIRRORED_PPU_REGISTER_END 0x4000
#define PRG_RAM_START 0x8000
#define PRG_RAM_END   0xFFFF


class Bus
{
	std::vector<BYTE> m_CPURAM{};
	ROM* m_ROM{};

	WORD MirrorAddress(WORD addr, WORD size, WORD startAddr = 0x0);
	BYTE ReadPPUByte(PPURegisters::Registers PPUReg);
	BYTE WritePPUByte(PPURegisters::Registers PPUreg);
	BYTE ReadPRGByte(WORD addr);
	WORD ReadPRGWord(WORD addr, bool shouldWrapPage = false);

public:
	Bus(ROM* rom);

	void Reset();

	BYTE ReadByte(WORD addr);
	WORD ReadWord(WORD addr, bool shouldWrapPage = false);
	void WriteByte(WORD addr, BYTE val);
	void WriteWord(WORD addr, WORD val);
};
