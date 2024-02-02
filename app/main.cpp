#include <thread>
#include <filesystem>

#include "Loader.h"
#include "RAM.h"
#include "CPU.h"
#include "PPU.h"
#include "Snake.h"

#define KB 1024

int main()
{
    constexpr size_t CPU_RAM_SIZE = 64*KB;
    constexpr WORD START_ADDR = 0x0600;
    
    RAM ram(CPU_RAM_SIZE);
    ram.WriteWord(CPU::RESET_VECTOR, START_ADDR); // start the program at specific mem location
    CPU cpu(&ram);
    //PPU ppu(&ram);
    
    auto bytes = Loader::LoadFile("/home/pai/github/emu/app/snake.bin");

    // map program directly into memory
    for (int i = 0; i < bytes.size(); ++i)
    {
        ram.WriteByte((WORD)(START_ADDR + i), bytes.at(i));
    }

    Snake snake(&ram);
    
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::microseconds(50));
        if (snake.Run())
            cpu.Run();
    }

    return 0;
}