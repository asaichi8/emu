#include "PPU.h"


PPU::PPU()
{
	m_PPURAM.assign(2 * KB, 0);
    m_OAM.assign(BYTE_MAX + 1, 0); // https://www.nesdev.org/wiki/PPU_memory_map#OAM
}