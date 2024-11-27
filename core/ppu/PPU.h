#pragma once

#include "IPPURegister.h"
#include "../memory/Bus.h"


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
