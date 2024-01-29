#include "../core/loader/Loader.h"
#include "../core/memory/CPUmem.h"
#include "../core/cpu/CPU.h"
#include "../core/ppu/PPU.h"

#define KB 1024

int main()
{
    constexpr size_t CPU_RAM_SIZE = 64*KB;
    constexpr WORD START_ADDR = 0x8000;

    //RAM ram(CPU_RAM_SIZE);
    CPUmem ram(CPU_RAM_SIZE);
    ram.WriteWord(CPU::RESET_VECTOR, START_ADDR); // start the program at specific mem location
    CPU cpu(&ram);
    PPU ppu;
    
    auto bytes = Loader::HexStrToBytes("");

    for (int i = 0; i < bytes.size(); ++i)
    {
        ram.WriteByte((WORD)(START_ADDR + i), bytes.at(i));
    }

    cpu.Run();

    return 0;
}