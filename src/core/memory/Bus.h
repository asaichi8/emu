#pragma once

#include <vector>
#include <memory>
#include "../ppu/PPU.h"
#include "../loader/ROM.h"
#include "../loader/Loader.h"
#include "../../include/typedefs.h"
#include "../ppu/registers/IPPURegister.h"
#include "../core/controller/Joypad.h"

// https://www.nesdev.org/wiki/CPU_memory_map
#define INTERNAL_RAM_SIZE 0x800				// 2KB of CPU's internal RAM
#define MIRRORED_INTERNAL_RAM_END 0x2000	// Address that the CPU's internal RAM's mirroring ends
#define PPU_REGISTER_SIZE 0x08				// Total size of the PPU's registers address range
#define MIRRORED_PPU_REGISTER_END 0x4000	// Address that the PPU registers mirroring ends
#define PRG_RAM_START 0x8000				// Address where the program ROM usually begins
#define PRG_RAM_END   0xFFFF				// Address where the program ROM usually ends


using PPURegAddr = IPPURegister::IPPURegister::PPURegAddr;

class PPU;

class Bus
{
	std::unique_ptr<PPU> m_PPU{};
	std::vector<BYTE> m_CPURAM{};
	ROM* m_ROM{};
	QWORD m_nCPUCycles{};
	Joypad m_Joypads[2];

	BYTE ReadPPURegister(PPURegAddr PPUreg);
	void WritePPURegister(PPURegAddr PPUreg, BYTE val);
	BYTE ReadPRGByte(WORD addr);
	WORD ReadPRGWord(WORD addr, bool shouldWrapPage = false);
	bool ApplyGameGenieCode(WORD addr);

public:
	Bus(ROM* rom);


	static WORD MirrorAddress(WORD addr, WORD size, WORD startAddr = 0x0);

	void Reset();
	void Clock(DWORD nCycles);
	BYTE ReadByte(WORD addr);
	void WriteByte(WORD addr, BYTE val);
	WORD ReadWord(WORD addr, bool shouldWrapPage = false);
	void WriteWord(WORD addr, WORD val);
	void UpdateJoypad(size_t joypad, Joypad::Button button, bool isDown);

	bool IsNMIInterruptQueuedW();
	const PPU* GetPPU() const { return m_PPU.get(); }
};
