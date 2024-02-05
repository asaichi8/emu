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
#include "Path.h"

constexpr int SCREEN_BUFFER_SIZE = SIZE * SIZE * 3;

/*void LoadProgramIntoRAM(Bus* bus, const std::string& filepath) 
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
}*/

#include <chrono>
#include <thread>
void GameLoop(Snake* snake, CPU* cpu, ROM* rom, std::unique_ptr<SnakeGUI>& gui) 
{
    SDL_Event event{};
    BYTE screenBuffer[SCREEN_BUFFER_SIZE]{};
    int FPS = 60;

    // get ui render rate
    SDL_DisplayMode currentDisplay{};
    int display = 0; // primary display
    if (SDL_GetCurrentDisplayMode(display, &currentDisplay) == 0)
        FPS = currentDisplay.refresh_rate;
    
    auto FPStime = std::chrono::milliseconds(1000) / FPS;
    auto nextFrameTime = std::chrono::high_resolution_clock::now() + FPStime;

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

        auto now = std::chrono::high_resolution_clock::now();
        if (now >= nextFrameTime) // only render gui every FPS frames (dependant on monitor hz)
        {
            gui->RenderFrame(screenBuffer, SIZE);
            nextFrameTime += FPStime;

            if (now > nextFrameTime)
                nextFrameTime = now + FPStime;
        }

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

    gui->InitImGui();
    GameLoop(&snake, &cpu, &rom, gui);
    gui->ShutdownImGui();

    return 0;
}