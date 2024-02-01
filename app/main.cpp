#include <thread>

#include "../core/loader/Loader.h"
#include "../core/memory/CPUmem.h"
#include "../core/cpu/CPU.h"
#include "../core/ppu/PPU.h"
#include "Snake.h"

#define KB 1024

int main()
{
    constexpr size_t CPU_RAM_SIZE = 64*KB;
    constexpr WORD START_ADDR = 0x0600;

    RAM ram(CPU_RAM_SIZE);
    ram.WriteWord(CPU::RESET_VECTOR, START_ADDR); // start the program at specific mem location
    CPU cpu(&ram);
    PPU ppu;
    
    std::string assem = "20 06 06 20 38 06 20 0D 06 20 2A 06 60 A9 02 85 02 A9 04 85 03 A9 11 85 10 A9 10 85 12 A9 0F 85 14 A9 04 85 11 85 13 85 15 60 A5 FE 85 00 A5 FE 29 03 18 69 02 85 01 60 20 4D 06 20 8D 06 20 C3 06 20 19 07 20 20 07 20 2D 07 4C 38 06 A5 FF C9 77 F0 0D C9 64 F0 14 C9 73 F0 1B C9 61 F0 22 60 A9 04 24 02 D0 26 A9 01 85 02 60 A9 08 24 02 D0 1B A9 02 85 02 60 A9 01 24 02 D0 10 A9 04 85 02 60 A9 02 24 02 D0 05 A9 08 85 02 60 60 20 94 06 20 A8 06 60 A5 00 C5 10 D0 0D A5 01 C5 11 D0 07 E6 03 E6 03 20 2A 06 60 A2 02 B5 10 C5 10 D0 06 B5 11 C5 11 F0 09 E8 E8 E4 03 F0 06 4C AA 06 4C 35 07 60 A6 03 CA 8A B5 10 95 12 CA 10 F9 A5 02 4A B0 09 4A B0 19 4A B0 1F 4A B0 2F A5 10 38 E9 20 85 10 90 01 60 C6 11 A9 01 C5 11 F0 28 60 E6 10 A9 1F 24 10 F0 1F 60 A5 10 18 69 20 85 10 B0 01 60 E6 11 A9 06 C5 11 F0 0C 60 C6 10 A5 10 29 1F C9 1F F0 01 60 4C 35 07 A0 00 A5 FE 91 00 60 A2 00 A9 01 81 10 A6 03 A9 00 81 10 60 A2 00 EA EA CA D0 FB 60";
    auto bytes = Loader::HexStrToBytes(assem);

    for (int i = 0; i < bytes.size(); ++i)
    {
        ram.WriteByte((WORD)(START_ADDR + i), bytes.at(i));
    }

    Snake snake(&ram);

    cpu.Run([&]() 
        {
            snake.Run();
            std::this_thread::sleep_for(std::chrono::microseconds(50));
        });

    return 0;
}