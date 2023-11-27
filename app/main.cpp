#include <iostream>
#include <cstring>
#include "../core/cpu/CPU.h"

#define KB 1024
#define EG1 1

int main()
{
    constexpr size_t CPU_RAM_SIZE = 64*KB;
    RAM ram(CPU_RAM_SIZE);
    ram.WriteWord(0xFFFC, 0x1000); // start the program at mem location 0x1000
    CPU cpu(&ram);
    
    // example code
    
#ifdef EG1
    ram.WriteByte(0x1000, 0xA2); // LDX
    ram.WriteByte(0x1001, 0x0A); // #$0A
    ram.WriteByte(0x1002, 0xE8); // INX
    ram.WriteByte(0x1003, 0x8E); // STX
    ram.WriteWord(0x1004, 0x2000); // $2000
    ram.WriteByte(0x1006, 0x02); // KIL (illegal opcode - halts CPU)
#endif

#ifndef EG1
    {
        const char* sz = "Hello, World!";
        for (int i = 0; i < strlen(sz); ++i)
        {
            ram.WriteByte(0x1000 + (i*6), 0xA2); // LDX
            ram.WriteByte(0x1001 + (i*6), sz[i]); // char as operand
            ram.WriteByte(0x1003 + (i*6), 0x8E); // STX
            ram.WriteWord(0x1004 + (i*6), 0x2000 + i); // storage location as operand

            if (i == strlen(sz) - 1) // if it's the last iteration...
                ram.WriteByte(0x1006 + (i*6), 0x02); // KIL (illegal opcode - halts CPU)
        }
    }
#endif
    

    cpu.Run();

#ifdef EG1
    std::cout << (int)(ram.ReadByte(0x2000)) << std::endl;
#endif

#ifndef EG1
    for (int i = 0; i < 13; ++i)
        std::cout << (char)(ram.ReadByte(0x2000 + i));
#endif

    return 0;
}