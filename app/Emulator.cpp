#include "Emulator.h"


Emulator::Emulator() 
{
    if (!m_ROM.LoadROM(PROGRAM_PATH))
        throw std::runtime_error("Failed to load ROM!");

    m_Bus   = std::make_shared<Bus>(&m_ROM);
    m_CPU   = std::make_unique<CPU>(m_Bus);
    m_PPU   = std::make_unique<PPU>(m_Bus);
    m_Snake = std::make_unique<Snake>(m_Bus);
    m_GUI   = std::make_unique<EmulatorDisplay>("Emulator", SIZE, SIZE, SCALE);

    m_GUI->InitImGui();
}

Emulator::~Emulator()
{
    m_GUI->ShutdownImGui();
}


void Emulator::Run() 
{
    SDL_Event event{};
    BYTE screenBuffer[SCREEN_BUFFER_SIZE]{};

    // get ui render rate
    SDL_DisplayMode currentDisplay{};
    int display = 0; // primary display
    if (SDL_GetCurrentDisplayMode(display, &currentDisplay) == 0)
        m_FPS = currentDisplay.refresh_rate;
    
    auto FPStime = std::chrono::milliseconds(1000) / m_FPS;
    auto nextFrameTime = std::chrono::high_resolution_clock::now() + FPStime;

    while (true) 
    {
        while (SDL_PollEvent(&event)) 
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            m_Snake->HandleEvent(event);
        }

        m_Snake->Run(screenBuffer);

        if (m_GUI->GetShouldReadRegisters()) 
            m_GUI->UpdateRegisters(m_CPU->ReadRegisters());

        auto now = std::chrono::high_resolution_clock::now();
        if (now >= nextFrameTime) // only render gui every FPS frames (dependant on monitor hz)
        {
            m_GUI->RenderFrame(screenBuffer, SIZE);
            nextFrameTime += FPStime;

            if (now > nextFrameTime)
                nextFrameTime = now + FPStime;
        }

        if (m_GUI->GetShouldCPURun()) 
            m_CPU->Run();
        if (m_GUI->GetShouldRestart())
        {
            m_CPU->Reset();
            m_GUI->SetShouldRestart(false);
        }
        if (m_GUI->GetShouldStepThrough())
        {
            m_CPU->Run();
            m_GUI->SetShouldStepThrough(false);
        }
        
        std::this_thread::sleep_for(std::chrono::microseconds(50));
    }
}