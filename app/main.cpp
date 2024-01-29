#include "../core/loader/Loader.h"
#include "../core/cpu/CPU.h"

#define KB 1024

int main()
{
    constexpr size_t CPU_RAM_SIZE = 64*KB;
    constexpr WORD START_ADDR = 0x0400;

    RAM ram(CPU_RAM_SIZE);
    ram.WriteWord(CPU::RESET_VECTOR, START_ADDR); // start the program at specific mem location
    CPU cpu(&ram);
    
    auto bytes = Loader::LoadFile("/home/---/Downloads/6502_functional_test.bin");

    for (int i = 0; i < bytes.size(); ++i)
    {
        ram.WriteByte((WORD)(i), bytes.at(i));
    }

    cpu.Run();

    return 0;
}