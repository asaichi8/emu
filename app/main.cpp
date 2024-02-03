#include <thread>
#include <filesystem>
#include <memory>
#include "Loader.h"
#include "RAM.h"
#include "CPU.h"
#include "PPU.h"
#include "Snake.h"
#include "SnakeGUI.h"

constexpr size_t KB = 1024;
constexpr size_t CPU_RAM_SIZE = 64 * KB;
constexpr WORD START_ADDR = 0x0600;
constexpr char PROGRAM_PATH[] = "/home/pai/github/emu/app/snake.bin";
constexpr int FRAME_DELAY_MICROSECONDS = 50;

void LoadProgramIntoRAM(RAM& ram, const std::string& filepath) 
{
    auto bytes = Loader::LoadFile(filepath);
    
    for (size_t i = 0; i < bytes.size(); ++i) 
    {
        ram.WriteByte(START_ADDR + i, bytes[i]);
    }
}

void InitializeRAM(RAM& ram) 
{
    ram.WriteWord(CPU::RESET_VECTOR, START_ADDR);
}

void GameLoop(Snake& snake, CPU& cpu, std::unique_ptr<SnakeGUI>& gui, BYTE* screenBuffer) 
{
    SDL_Event event;
    
    while (true) 
    {
        while (SDL_PollEvent(&event)) 
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            snake.HandleEvent(event);
        }

        snake.Run(screenBuffer);
        
        gui->RenderFrame(screenBuffer, SIZE);

        if (gui->GetShouldCPURun()) 
            cpu.Run();

        //std::this_thread::sleep_for(std::chrono::microseconds(50));
    }
}

int main() 
{
    RAM ram(CPU_RAM_SIZE);
    InitializeRAM(ram);
    CPU cpu(&ram);
    LoadProgramIntoRAM(ram, PROGRAM_PATH);

    Snake snake(&ram);
    std::unique_ptr<SnakeGUI> gui = std::make_unique<SnakeGUI>(SIZE, SIZE, SCALE);
    BYTE screenBuffer[SIZE * SIZE * 3]{};

    gui->InitImGui();
    GameLoop(snake, cpu, gui, screenBuffer);
    gui->ShutdownImGui();

    return 0;
}