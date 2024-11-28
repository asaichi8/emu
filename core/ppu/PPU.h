#pragma once

#include "../memory/Bus.h"
#include "registers/IPPURegister.h"


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
	
public:
	PPU();

	Registers registers;
};
