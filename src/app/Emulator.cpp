#include "Emulator.h"
// TODO: add controller saving/remapping, add joypad2
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
	std::string errMsg = m_ROM.LoadROM(m_curRomPath);
	if (!errMsg.empty())
		throw std::runtime_error("Failed to load ROM : " + errMsg);

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
		{
			m_GUI->RenderFrame(nesDisplay.GetScreen(), DISPLAY_WIDTH);
			loopStart = std::chrono::high_resolution_clock::now();;
		}


		if (m_GUI->GetShouldReadRegisters())				// if read registers was clicked...
			m_GUI->UpdateRegisters(m_CPU->ReadRegisters()); // update GUI with a copy of the CPU's current registers

		if (m_GUI->GetShouldRestart())
		{
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
 
		if (m_GUI->GetSelectedFile() != "")
		{
			std::string strSelectedFile = m_GUI->GetSelectedFile();
			m_GUI->SetSelectedFile("");

			std::string errMsg = m_ROM.CheckROM(strSelectedFile);
			if (!errMsg.empty())
				m_GUI->SetShouldShowErrorMsg(true, errMsg, "Failed to load file");
			else
				return strSelectedFile; // file drop succeeded
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

				switch (event.type)
				{
					case SDL_QUIT:
						return {};

					// === FILE HANDLING ===
					// https://wiki.libsdl.org/SDL2/SDL_DropEvent
					case SDL_DROPFILE:
					{
						if (m_GUI->GetShouldShowErrorMsg()) 
							break;
							
						char* droppedFile = event.drop.file; // MUST BE FREED WITH SDL_free
						std::string strDroppedFile = droppedFile;
						SDL_free(droppedFile);

						if (strDroppedFile.empty()) // no need to check if valid before pushing to recent files, only if empty
							m_GUI->m_recentFiles.Push(strDroppedFile);

						std::string errMsg = m_ROM.CheckROM(strDroppedFile);
						if (!errMsg.empty())
						{
							m_GUI->SetShouldShowErrorMsg(true, errMsg, "Failed to load file");
							break;
						}

						return strDroppedFile; // file drop succeeded
					}

					// === CONTROLLER CONNECTION HANDLING ===
					case SDL_CONTROLLERDEVICEADDED:
					case SDL_CONTROLLERDEVICEREMOVED:
					case SDL_CONTROLLERDEVICEREMAPPED:
						m_GUI->GetControllerHandler()->UpdateControllers();
						break;

					// === BUTTON HANDLING ===
					case SDL_KEYDOWN:
					case SDL_KEYUP:
					{
						if (m_GUI->GetShouldShowErrorMsg() || !m_GUI->GetShouldCPURun()) 
							break; // block input if error msg showing

						auto iterator = m_keyButtonMap.find((SDL_KeyCode)event.key.keysym.sym);
						if (iterator != m_keyButtonMap.end()) 
							m_Bus->UpdateJoypad(0, iterator->second, event.type == SDL_KEYDOWN ? true : false); 

						break;
					}

					case SDL_CONTROLLERBUTTONDOWN:
					case SDL_CONTROLLERBUTTONUP:
					{
						if (m_GUI->GetShouldShowErrorMsg() || !m_GUI->GetShouldCPURun()) 
							break; // block input if error msg showing

						Ports* ports = &m_GUI->GetControllerHandler()->m_Ports;
						for (size_t portNo = 1; portNo < 3; ++portNo)
						{
							if (ports->GetJoystickID(portNo) != event.cbutton.which)
								continue;
							
							auto iterator = m_controllerButtonMap.find((SDL_GameControllerButton)event.cbutton.button);
							if (iterator != m_controllerButtonMap.end()) 
								m_Bus->UpdateJoypad(portNo - 1, iterator->second, event.type == SDL_CONTROLLERBUTTONDOWN ? true : false); 
						}

						break;
					}

					default:
						break;
				}
			}
		}
		// std::this_thread::sleep_for(std::chrono::microseconds(50));
	}

	return m_curRomPath;
}
