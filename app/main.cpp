#include <thread>
#include <filesystem>
#include <algorithm>
#include <memory>
#include "Loader.h"
#include "CPU.h"
#include "PPU.h"
#include "Snake.h"
#include "SnakeGUI.h"

#include "ROM.h"

constexpr size_t KB = 1024;
constexpr size_t CPU_RAM_SIZE = 64 * KB;
constexpr WORD START_ADDR = 0xC000;
constexpr char PROGRAM_PATH[] = "/home/---/github/emu/app/snake.bin";
constexpr int FRAME_DELAY_MICROSECONDS = 50;
constexpr int SCREEN_BUFFER_SIZE = SIZE * SIZE * 3;

void LoadProgramIntoRAM(Bus* bus, const std::string& filepath) 
{
    auto bytes = Loader::LoadFile(filepath);
    
    for (size_t i = 0; i < bytes.size(); ++i) 
    {
        bus->WriteByte(START_ADDR + i, bytes[i]);
    }
}

void InitializeRAM(Bus* bus) 
{
    bus->WriteWord(CPU::RESET_VECTOR, START_ADDR);
}

void GameLoop(Snake* snake, CPU* cpu, ROM* rom, std::unique_ptr<SnakeGUI>& gui, BYTE* screenBuffer) 
{
    SDL_Event event;
    
    while (true) 
    {
        while (SDL_PollEvent(&event)) 
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            snake->HandleEvent(event);
        }

        snake->Run(screenBuffer);

        if (gui->GetShouldReadRegisters()) 
            gui->UpdateRegisters(cpu->ReadRegisters());

        gui->RenderFrame(screenBuffer, SIZE);

        if (gui->GetShouldCPURun()) 
            cpu->Run();
        if (gui->GetShouldRestart())
        {
            cpu->Reset();
            gui->SetShouldRestart(false);
        }
        if (gui->GetShouldStepThrough())
        {
            cpu->Run();
            gui->SetShouldStepThrough(false);
        }
        

        //std::this_thread::sleep_for(std::chrono::microseconds(50));
    }
}

int main() 
{
    ROM rom;
    if (!rom.LoadROM(PROGRAM_PATH))
        std::cout << "bad";
    Bus bus(&rom);
    CPU cpu(&bus);
    Snake snake(&bus);

    std::unique_ptr<SnakeGUI> gui = std::make_unique<SnakeGUI>(SIZE, SIZE, SCALE);
    BYTE screenBuffer[SCREEN_BUFFER_SIZE]{};

    gui->InitImGui();
    GameLoop(&snake, &cpu, &rom, gui, screenBuffer);
    gui->ShutdownImGui();

    return 0;
}