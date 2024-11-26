#include "PPU.h"


PPU::PPU(std::shared_ptr<Bus> bus) : m_Bus(bus)
{
	m_PPURAM.assign(2 * KB, 0);
    m_OAM.assign(BYTE_MAX + 1, 0); // https://www.nesdev.org/wiki/PPU_memory_map#OAM
}