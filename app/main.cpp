#include <thread>
#include <filesystem>

#include "Loader.h"
#include "RAM.h"
#include "CPU.h"
#include "PPU.h"
#include "Snake.h"
#include "SDLApp.h"

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
    
    std::unique_ptr<SDLApp> m_app = std::make_unique<SDLApp>("Snake", SIZE, SIZE, SCALE);
    uint8_t m_Screen[SIZE * SIZE * 3]{};
    SDL_Event e{};
    m_app->InitImGui();
    while (true)
    {
        while (SDL_PollEvent(&e))
        {
            ImGui_ImplSDL2_ProcessEvent(&e);
            snake.HandleEvent(e);
        }

        snake.Run(m_Screen);

        // render gui
        SDL_RenderClear(m_app->GetRenderer());
        
        SDL_UpdateTexture(m_app->GetTexture(), NULL, m_Screen, SIZE * 3);
        SDL_RenderCopy(m_app->GetRenderer(), m_app->GetTexture(), NULL, NULL);
        m_app->StartImGuiFrame();
        m_app->RenderImGuiFrame();

        SDL_RenderPresent(m_app->GetRenderer());

        if (m_app->GetShouldCPURun())
            cpu.Run();

        std::this_thread::sleep_for(std::chrono::microseconds(50));
    }
    m_app->ShutdownImGui();

    return 0;
}