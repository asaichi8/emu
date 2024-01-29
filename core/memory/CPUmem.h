#include "RAM.h"

// https://www.nesdev.org/wiki/CPU_memory_map
#define INTERNAL_RAM_SIZE 0x800
#define MIRRORED_INTERNAL_RAM_END 0x2000
#define PPU_REGISTER_SIZE 0x08
#define MIRRORED_PPU_REGISTER_END 0x4000

class CPUmem : public RAM
{

public:
    CPUmem(size_t size) : RAM(size) {}
    //BYTE ReadByte(WORD addr) override;
    //WORD ReadWord(WORD addr, bool shouldWrapPage = false) override;
    void WriteByte(WORD addr, BYTE val) override;
    void WriteWord(WORD addr, WORD val) override;
};