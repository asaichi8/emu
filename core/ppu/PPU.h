#pragma once

#include "../memory/Bus.h"
#include "registers/IPPURegister.h"


class PPU
{
	std::vector<BYTE> m_PPURAM{};
	std::vector<BYTE> m_OAM{};
	/*const std::vector<std::unique_ptr<IPPURegister>> registers {
		std::make_unique<PPUCTRL>()	
	};*/
	
public:
	PPU();
};
