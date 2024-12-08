#include "Emulator.h"
// TODO: ADD COLOUR, ADD SPRITES, ADD CONTROLLER SUPPORT
// TODO: make DrawTiles popup in ImGUI

// TODO: add palette loader/rom loader (on drag, cmdline)
// TODO: add define for enabling/disabling debug rom

// TODO: use unordered_map for enums
// TODO: add logging, remove std::cerrs/add verbose option
// TODO: make a bunch of parameters const and stuff
// TODO: avoid define, use static const member variables
// TODO: make sure all member variables are m_, use regular naming scheme
// TODO: make private functions in .cpp at bottom, public at top

Emulator::Emulator(const std::string& romPath)
{
	// Map ROM into appropriate variables
	std::string romFullPath = Loader::GetFullFilePath(romPath.c_str());
	if (!m_ROM.LoadROM(romFullPath))
		throw std::runtime_error("Failed to load ROM!");

	m_pPalette = std::make_unique<Palette>();
	std::string paletteFullPath = Loader::GetFullFilePath(PALETTE_RELATIVE_PATH);
	if (!m_pPalette->LoadPalette(paletteFullPath))
		throw std::runtime_error("Failed to load colour palette!");

	// Create devices
	m_Bus = std::make_shared<Bus>(&m_ROM);
	m_CPU = std::make_unique<CPU>(m_Bus);
	m_GUI = std::make_unique<EmulatorDisplay>("Emulator", DISPLAY_WIDTH, DISPLAY_HEIGHT, 1);

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
	NESDisplay nesDisplay(m_Bus->GetPPU(), m_pPalette.get());

	// Determine refresh rate of primary monitor
	//SDL_DisplayMode currentDisplay{};
	//int primaryDisplay = 0; // primary display
	// Attempt to set FPS to the refresh rate of the focused monitor
	//if (SDL_GetCurrentDisplayMode(primaryDisplay, &currentDisplay) == 0)
	//	m_FPS = currentDisplay.refresh_rate;
	//auto FPStime = std::chrono::milliseconds(1000) / 60;					  // time each frame should run
	//auto nextFrameTime = std::chrono::high_resolution_clock::now() + FPStime; // time the next frame will be drawn

	bool running = true;
	while (running) // Main loop
	{

		if (m_GUI->GetShouldReadRegisters())				// if read registers was clicked...
			m_GUI->UpdateRegisters(m_CPU->ReadRegisters()); // update GUI with a copy of the CPU's current registers

		if (m_GUI->GetShouldCPURun())
		{
			m_CPU->Run();

			// If we interrupted, it's probably safe to read the nametable so try it
			// TODO: very bootleg fix later
			// static bool hasInterrupted = false;
			// if (!m_Bus->GetPPU()->GetInterruptStatus()) 
			// 	hasInterrupted = false;

			if (m_Bus->GetPPU()->GetInterruptStatus())
			{
				nesDisplay.DrawScreen();
				m_GUI->RenderFrame(nesDisplay.GetScreen(), DISPLAY_WIDTH);
				// hasInterrupted = true;
			}
		}
		else
			m_GUI->RenderFrame(nesDisplay.GetScreen(), DISPLAY_WIDTH);

		/*auto now = std::chrono::high_resolution_clock::now();
		if (now >= nextFrameTime) // only render gui every FPS frames
		{
			m_GUI->RenderFrame(screenBuffer, DISPLAY_WIDTH);
			nextFrameTime += FPStime; // calc the time the next frame should be drawn

			if (now > nextFrameTime) // if somehow we became out of sync...
				nextFrameTime = now + FPStime; // synchronise nextFrameTime
		}*/

		// TODO: fix restart
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

		// Handle events (e.g. keyboard inputs)
		while (SDL_PollEvent(&event))
		{
			ImGui_ImplSDL2_ProcessEvent(&event);
			if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
			{
				running = false;
				break;
			}

			auto iterator = m_buttonMap.find((SDL_KeyCode)event.key.keysym.sym);
			if (iterator != m_buttonMap.end())
			{
				// set key if it's down, unset if it's not
				if (event.type == SDL_KEYDOWN)
					m_Bus->joypad1.Update(iterator->second, true); 
				else if (event.type == SDL_KEYUP)
					m_Bus->joypad1.Update(iterator->second, false);
			}
		}
		// std::this_thread::sleep_for(std::chrono::microseconds(50));
	}
}
