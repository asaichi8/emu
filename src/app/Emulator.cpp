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

Emulator::Emulator(const std::string& romPath, EmulatorDisplay& GUI) : m_GUI(&GUI)
{
	// Map ROM into appropriate variables
	m_curRomPath = Loader::GetFullFilePath(romPath.c_str());
	if (!m_ROM.LoadROM(m_curRomPath))
		throw std::runtime_error("Failed to load ROM!");

	m_pPalette = std::make_unique<Palette>();
	std::string paletteFullPath = Loader::GetFullFilePath(PALETTE_RELATIVE_PATH);
	if (!m_pPalette->LoadPalette(paletteFullPath))
		throw std::runtime_error("Failed to load colour palette!");

	// Create devices
	m_Bus = std::make_shared<Bus>(&m_ROM);
	m_CPU = std::make_unique<CPU>(m_Bus);
}

Emulator::~Emulator()
{
	
}


/// @brief Runs the emulator.
/// @return The next ROM file to run. Returns nothing if we expect to exit.
std::string Emulator::Run()
{
	bool shouldRestart = false;

	SDL_Event event{};
	NESDisplay nesDisplay(m_Bus->GetPPU(), m_pPalette.get());

	// Determine refresh rate of primary monitor
	//SDL_DisplayMode currentDisplay{};
	//int primaryDisplay = 0; // primary display
	// Attempt to set FPS to the refresh rate of the focused monitor
	//if (SDL_GetCurrentDisplayMode(primaryDisplay, &currentDisplay) == 0)
	//	m_FPS = currentDisplay.refresh_rate;
	// TODO: make sure the timers dont go out of sync
	const auto FPStime = std::chrono::milliseconds(1000) / 60;					  // time each frame should run
	//auto nextFrameTime = std::chrono::high_resolution_clock::now() + FPStime; // time the next frame will be drawn
	const int BATCH_MULTIPLIER = 179; //number of cpu cycles to execute at a time
	const double CPU_CLOCK_RATE = 1789773 / 2.5; // CPU clock rate divided by arbitrary number because this is wrong
	const double CYCLE_TIME = 1e6 / CPU_CLOCK_RATE * BATCH_MULTIPLIER; // working with microseconds

	// controller poll time = 1ms
	bool running = true;
	auto now = std::chrono::high_resolution_clock::now();
	while (running) // Main loop
	{
		static auto loopStart = std::chrono::high_resolution_clock::now();
		static auto nextFrameTime = std::chrono::high_resolution_clock::now() + FPStime;

		// TODO: this lags when unpausing, probably because elapsed gets really high when paused
		if (m_GUI->GetShouldCPURun())
		{
			now = std::chrono::high_resolution_clock::now();
			double elapsed = std::chrono::duration<double, std::micro>(std::chrono::high_resolution_clock::now() - loopStart).count();
			
			double batchamnt = elapsed / CYCLE_TIME;

			for (int i = 0; i < std::round(batchamnt); ++i)
			{
				for (int j = 0; j < BATCH_MULTIPLIER; ++j)
				{
					m_CPU->Run();

					if (m_Bus->GetPPU()->GetInterruptStatus())
						nesDisplay.DrawScreen();
				}

				loopStart += std::chrono::microseconds((int)(CYCLE_TIME));
			}


			// render at 60fps
			now = std::chrono::high_resolution_clock::now();
			if (now >= nextFrameTime)
			{
				nextFrameTime = now + FPStime;
				m_GUI->RenderFrame(nesDisplay.GetScreen(), DISPLAY_WIDTH);
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

		if (m_GUI->GetShouldReadRegisters())				// if read registers was clicked...
			m_GUI->UpdateRegisters(m_CPU->ReadRegisters()); // update GUI with a copy of the CPU's current registers

		// TODO: fix restart
		if (m_GUI->GetShouldRestart())
		{
			//m_CPU->Reset();
			m_GUI->SetShouldRestart(false);
			shouldRestart = true;
			running = false;
			break;
		}
		if (m_GUI->GetShouldStepThrough())
		{
			m_CPU->Run(); // execute the CPU for a single instruction
			m_GUI->SetShouldStepThrough(false);
		}

		static auto nextEventCheck = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(1);
		now = std::chrono::high_resolution_clock::now();
		// Handle events (e.g. keyboard inputs)
		if (now >= nextEventCheck)
		{
			nextEventCheck = now + std::chrono::milliseconds(1);

			while (SDL_PollEvent(&event))
			{
				ImGui_ImplSDL2_ProcessEvent(&event);
				if ((event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) ||
					 event.type == SDL_QUIT)
				{
					return {};
				}

				// TODO: fix socd
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
		}
		// std::this_thread::sleep_for(std::chrono::microseconds(50));
	}

	return m_curRomPath;
}
