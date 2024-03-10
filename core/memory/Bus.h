#pragma once

#include <vector>
#include <memory>
#include "ROM.h"
#include "typedefs.h"

// https://www.nesdev.org/wiki/CPU_memory_map
#define INTERNAL_RAM_SIZE 0x800
#define MIRRORED_INTERNAL_RAM_END 0x2000
#define PPU_REGISTER_SIZE 0x08
#define MIRRORED_PPU_REGISTER_END 0x4000
#define PRG_RAM_START 0x8000
#define PRG_RAM_END   0xFFFF


class Bus
{
	std::vector<BYTE> m_RAM{};
	ROM* m_ROM{};

public:
	Bus(ROM* rom);

	void Reset();

	BYTE ReadPRGByte(WORD addr);
	WORD ReadPRGWord(WORD addr, bool shouldWrapPage = false);

	BYTE ReadByte(WORD addr);
	WORD ReadWord(WORD addr, bool shouldWrapPage = false);
	void WriteByte(WORD addr, BYTE val);
	void WriteWord(WORD addr, WORD val);
};