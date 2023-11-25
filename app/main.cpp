#include <iostream>
#include "../core/cpu/CPU.h"


int main()
{
    RAM ram;
    CPU cpu(&ram);

    // example code
    ram.WriteByte(0, 0x5);
    std::cout << (int)(ram.ReadByte(0)) << std::endl;

    return 0;
}