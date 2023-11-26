#include <iostream>
#include "../core/cpu/CPU.h"

#define KB 1024

int main()
{
    constexpr size_t CPU_RAM_SIZE = 64*KB;
    RAM ram(CPU_RAM_SIZE);
    CPU cpu(&ram);

    // example code
    ram.WriteByte(0, 0x5);
    std::cout << (int)(ram.ReadByte(0)) << std::endl;

    return 0;
}