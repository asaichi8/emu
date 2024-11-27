#pragma once

#include "../memory/Bus.h"
#include "registers/IPPURegister.h"


class PPU
{
	std::vector<BYTE> m_PPURAM{};
	std::vector<BYTE> m_OAM{};
	struct Registers {
		std::unique_ptr<IPPURegister> ppuctrl 	= std::make_unique<PPUCTRL>	 ();
		std::unique_ptr<IPPURegister> ppumask 	= std::make_unique<PPUMASK>	 ();
		std::unique_ptr<IPPURegister> ppustatus = std::make_unique<PPUSTATUS>();
		std::unique_ptr<IPPURegister> oamaddr 	= std::make_unique<OAMADDR>	 ();
		std::unique_ptr<IPPURegister> oamdata 	= std::make_unique<OAMDATA>	 ();
		std::unique_ptr<IPPURegister> ppuscroll = std::make_unique<PPUSCROLL>();
		std::unique_ptr<IPPURegister> ppuaddr 	= std::make_unique<PPUADDR>	 ();
		std::unique_ptr<IPPURegister> ppudata 	= std::make_unique<PPUDATA>	 ();
		std::unique_ptr<IPPURegister> oamdata 	= std::make_unique<OAMDATA>	 ();
	} registers;	 
	
public:
	PPU();
};
