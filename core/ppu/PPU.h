#pragma once

#include "Bus.h"


class PPU
{
	std::shared_ptr<Bus> m_Bus{};
	
public:
	PPU(std::shared_ptr<Bus> bus);
};