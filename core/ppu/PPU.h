#pragma once

#include "../memory/Bus.h"
#include "registers/IPPURegister.h"

// https://www.nesdev.org/wiki/PPU_memory_map
#define PATTERN_TABLE_SIZE 0x1000
#define PATTERN_TABLE_TOTAL_SIZE (PATTERN_TABLE_SIZE * 2) // 0x2000
#define NAMETABLE_BEGIN 0x2000
#define NAMETABLE_SIZE 0x0400
#define NAMETABLE_TOTAL_SIZE (NAMETABLE_SIZE * 4) // 0x1000
#define UNUSED_BEGIN (PATTERN_TABLE_SIZE + NAMETABLE_TOTAL_SIZE) // 0x3000
#define UNUSED_TOTAL_SIZE 0x0F00
#define PALETTE_RAM_BEGIN (PATTERN_TABLE_SIZE + NAMETABLE_TOTAL_SIZE + UNUSED_BEGIN) // 0x3F00
#define PALETTE_RAM_SIZE 0x20
#define PPU_ADDRESS_SPACE_END 0x4000


class PPU
{
	std::vector<BYTE> m_PPURAM{};
	std::vector<BYTE> m_OAM{};

	IPPURegister::InternalRegisters internal_registers;

	struct Registers {
		std::unique_ptr<IPPURegister> ppuctrl 	;
		std::unique_ptr<IPPURegister> ppumask 	;
		std::unique_ptr<IPPURegister> ppustatus ;
		std::unique_ptr<IPPURegister> oamaddr 	;
		std::unique_ptr<IPPURegister> oamdata 	;
		std::unique_ptr<IPPURegister> ppuscroll ;
		std::unique_ptr<IPPURegister> ppuaddr 	;
		std::unique_ptr<IPPURegister> ppudata 	;
		std::unique_ptr<IPPURegister> oamdma 	;

		// TODO: should every PPU registers have access to its internal registers?
		Registers(IPPURegister::InternalRegisters* i)
		{
			ppuctrl 	= std::make_unique<PPUCTRL>	 (nullptr);
			ppumask 	= std::make_unique<PPUMASK>	 (nullptr);
			ppustatus 	= std::make_unique<PPUSTATUS>(i);
			oamaddr 	= std::make_unique<OAMADDR>	 (nullptr);
			oamdata 	= std::make_unique<OAMDATA>	 (nullptr);
			ppuscroll 	= std::make_unique<PPUSCROLL>(i);
			ppuaddr 	= std::make_unique<PPUADDR>	 (nullptr);
			ppudata 	= std::make_unique<PPUDATA>	 (i);
			oamdma 		= std::make_unique<OAMDMA>	 (nullptr);
		}
	};	 

	WORD GetPPUADDRAddress(bool shouldIncrement = false);
	
public:
	PPU();

	Registers registers;

	BYTE ReadPPUByte();
	void WritePPUByte(BYTE val);
};
