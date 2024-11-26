#pragma once

#include "../memory/Bus.h"


class PPU
{
	std::vector<BYTE> m_PPURAM{};
	std::vector<BYTE> m_OAM{};
	std::shared_ptr<Bus> m_Bus{};
	
public:
	PPU(std::shared_ptr<Bus> bus);
};
