#include "Emulator.h"


Emulator::Emulator() 
{
	// Map ROM into appropriate variables
	if (!m_ROM.LoadROM(PROGRAM_PATH))
		throw std::runtime_error("Failed to load ROM!");

	// Create devices
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


/// @brief Runs the emulator.
void Emulator::Run() 
{
	SDL_Event event{};
	BYTE screenBuffer[SCREEN_BUFFER_SIZE]{};

	// Determine refresh rate of primary monitor
	SDL_DisplayMode currentDisplay{};
	int primaryDisplay = 0; // primary display
	// Attempt to set FPS to the refresh rate of the focused monitor
	if (SDL_GetCurrentDisplayMode(primaryDisplay, &currentDisplay) == 0)
		m_FPS = currentDisplay.refresh_rate;
	
	auto FPStime = std::chrono::milliseconds(1000) / m_FPS; // time each frame should run
	auto nextFrameTime = std::chrono::high_resolution_clock::now() + FPStime; // time the next frame will be drawn

	while (true) // Main loop
	{
		// Handle events (e.g. keyboard inputs)
		while (SDL_PollEvent(&event)) 
		{
			ImGui_ImplSDL2_ProcessEvent(&event);
			m_Snake->HandleEvent(event);
		}

		m_Snake->Run(screenBuffer); // run snake class handler

		if (m_GUI->GetShouldReadRegisters()) // if read registers was clicked...
			m_GUI->UpdateRegisters(m_CPU->ReadRegisters()); // update GUI with a copy of the CPU's current registers

		auto now = std::chrono::high_resolution_clock::now();
		if (now >= nextFrameTime) // only render gui every FPS frames
		{
			m_GUI->RenderFrame(screenBuffer, SIZE);
			nextFrameTime += FPStime; // calc the time the next frame should be drawn

			if (now > nextFrameTime) // if somehow we became out of sync...
				nextFrameTime = now + FPStime; // synchronise nextFrameTime
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
			m_CPU->Run(); // execute the CPU for a single instruction
			m_GUI->SetShouldStepThrough(false);
		}
		
		std::this_thread::sleep_for(std::chrono::microseconds(50));
	}
}