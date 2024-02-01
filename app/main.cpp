#include <thread>
#include <filesystem>

#include "../core/loader/Loader.h"
#include "../core/memory/RAM.h"
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
    //PPU ppu(&ram);
    
    auto bytes = Loader::LoadFile(std::string(std::filesystem::current_path() / "snake.bin"));

    // map program directly into memory
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