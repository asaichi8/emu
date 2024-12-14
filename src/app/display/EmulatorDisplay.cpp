#include "EmulatorDisplay.h"


EmulatorDisplay::EmulatorDisplay(const std::string& winName, int w, int h, int scale, ControllerHandler* pCH) 
	: SDLApp(winName, w, h, scale), m_pControllerHandler(pCH)
{
	this->InitImGui();
}

EmulatorDisplay::~EmulatorDisplay()
{
	this->ShutdownImGui();
}


/// @brief Initialise the ImGui window and integrate it with the SDL2 renderer
void EmulatorDisplay::InitImGui() 
{
	if (!m_Window || !m_Renderer)       
		throw std::runtime_error("no window or renderer");

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.IniFilename = NULL;
	io.LogFilename = NULL;

	ImGui_ImplSDL2_InitForSDLRenderer(
		m_Window,
		m_Renderer
	);
	ImGui_ImplSDLRenderer2_Init(m_Renderer);
}

void EmulatorDisplay::OpenFileDialog()
{
	shouldShowFileDialog = true;
	// pause CPU while we're opening a file - we still need to render though
	bool preservedShouldCPURun = shouldCPURun;
	shouldCPURun = false;

	const char* filterPatterns[] = {"*.nes"};
	const char* filePath = tinyfd_openFileDialog("Select a file", "", 1, filterPatterns, "NES files", 0);

	if (filePath)
	{
		SetSelectedFile(filePath);
		m_recentFiles.Push(filePath);
	}
	
	shouldCPURun = preservedShouldCPURun;
	shouldShowFileDialog = false;
}

/// @brief Called every frame that the GUI is rendered - consists of the actual UI
void EmulatorDisplay::StartImGuiFrame() 
{
	// Begin a new frame for our window
	ImGui_ImplSDLRenderer2_NewFrame();
	ImGui_ImplSDL2_NewFrame(m_Window);

	ImGui::NewFrame();

	if (ImGui::BeginMainMenuBar()) 
	{
		if (ImGui::BeginMenu("File")) 
		{
			if (ImGui::MenuItem("Load file"))
			{
				std::thread t_FileDialog([this]() { OpenFileDialog(); });
				t_FileDialog.detach();
			}

			if (!m_recentFiles.GetQueue().empty() && ImGui::BeginMenu("Load recent file"))
			{
				for (const auto& path : m_recentFiles.GetQueue())
				{
					if (ImGui::MenuItem(path.c_str()))
					{
						SetSelectedFile(path);
						m_recentFiles.Push(path);
					}
				}
				ImGui::EndMenu();
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Restart")) 
				shouldRestart = true;

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Debug")) 
		{
			if (ImGui::MenuItem(shouldCPURun ? "Pause" : "Resume")) 
				shouldCPURun = !shouldCPURun;
				
			if (ImGui::MenuItem(shouldReadRegisters ? "Hide registers" : "Display registers"))
				shouldReadRegisters = !shouldReadRegisters;
				
			if (!shouldCPURun && ImGui::MenuItem("Step through")) 
				shouldStepThrough = true;
					
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Settings"))
		{
			if (ImGui::MenuItem("Controllers"))
				shouldOpenControllerWin = true;
			
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	if (shouldOpenControllerWin)
	{
		ImGui::Begin("Controller", &shouldOpenControllerWin, ImGuiWindowFlags_AlwaysAutoResize);

		// create combo for each port
		for (int i = 1; i <= m_pControllerHandler->m_Ports.GetPortSize(); ++i)
		{
			CreateControllerCombo(i);
		}

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		if (ImGui::Button("Save defaults", ImVec2(ImGui::GetContentRegionAvail().x, 0)))
		{
			Config* config = &Config::GetInstance();

			bool failedToRead = config->_file.read(config->ini);

			char szGuid[33];
			for (int i = 1; i <= m_pControllerHandler->m_Ports.GetPortSize(); ++i)
			{
				SDL_JoystickGetGUIDString(m_pControllerHandler->m_Ports.GetJoystickGUID(i), szGuid, sizeof(szGuid));
				config->ini["ports"]["port" + std::to_string(i)] = szGuid;
			}

			bool writeSuccess{};
			if (failedToRead)
				writeSuccess = config->_file.generate(config->ini, true);
			else
				writeSuccess = config->_file.write(config->ini, true);

			if (!writeSuccess)
				std::cerr << "Failed to write to file" << std::endl;
		}

		ImGui::End();
	}

	if (shouldReadRegisters)
	{
		static const ImVec4 green = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
		static const ImVec4 red   = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);

		ImGui::Begin("CPU", &shouldReadRegisters, ImGuiWindowFlags_AlwaysAutoResize);
		
		ImGui::Text("PC: 0x%04X", m_curReg.program_counter);
		ImGui::Text("SP: 0x%02X", m_curReg.stack_pointer);
		ImGui::Text("A:  0x%02X", m_curReg.accumulator);
		ImGui::Text("X:  0x%02X", m_curReg.X);
		ImGui::Text("Y:  0x%02X", m_curReg.Y);
		ImGui::Separator();
		ImGui::TextColored(m_curReg.status_register.test(StatusRegisterFlags::CARRY) ? green : red, "C"); ImGui::SameLine();
		ImGui::TextColored(m_curReg.status_register.test(StatusRegisterFlags::ZERO) ? green : red, "Z"); ImGui::SameLine();
		ImGui::TextColored(m_curReg.status_register.test(StatusRegisterFlags::INTERRUPT_REQUEST) ? green : red, "I"); ImGui::SameLine();
		ImGui::TextColored(m_curReg.status_register.test(StatusRegisterFlags::DECIMAL_MODE) ? green : red, "D"); ImGui::SameLine();
		ImGui::TextColored(m_curReg.status_register.test(StatusRegisterFlags::BREAK_COMMAND) ? green : red, "B"); ImGui::SameLine();
		ImGui::TextColored(m_curReg.status_register.test(StatusRegisterFlags::UNUSED) ? green : red, "U"); ImGui::SameLine();
		ImGui::TextColored(m_curReg.status_register.test(StatusRegisterFlags::_OVERFLOW) ? green : red, "O"); ImGui::SameLine();
		ImGui::TextColored(m_curReg.status_register.test(StatusRegisterFlags::NEGATIVE) ? green : red, "N");
		
		ImGui::End();
	}

	if (shouldShowErrorMsg)
	{
		ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		
		ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(1.f, .3f, .3f, .9f));
		ImGui::OpenPopup(m_lastErrorTitle.c_str());
		ImGui::SetNextWindowSize(ImVec2(200, 84));
		if (ImGui::BeginPopupModal(m_lastErrorTitle.c_str(), nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("%s", m_lastError.c_str());

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			if (ImGui::Button("OK", ImVec2(ImGui::GetContentRegionAvail().x, 0)))
			{
				ImGui::CloseCurrentPopup();
				shouldShowErrorMsg = false;
			}

			ImGui::EndPopup();
		}
		ImGui::PopStyleColor();
	}

	if (shouldShowFileDialog)
	{
		ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		
		static const char* title = "Select a file";
		ImGui::OpenPopup(title);
		if (ImGui::BeginPopupModal(title, nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Please choose a file.");
			ImGui::EndPopup();
		}
	}

	ImGui::Render();
}

void EmulatorDisplay::CreateControllerCombo(size_t port)
{
	ImGui::Text("Port %d: ", port);
	ImGui::SameLine();

	auto curController = m_pControllerHandler->m_Ports.Retrieve(port);

	// check if current controller is still connected
	// (we do this here because if it's disconnected, it can't write anything anyway)
	auto controllers = m_pControllerHandler->GetControllers();
	if (curController != nullptr && std::find(controllers.begin(), controllers.end(), curController) == controllers.end())
	{
		// not connected so disconnect
		m_pControllerHandler->m_Ports.Disconnect(port);
		curController = nullptr;
	}

	std::string comboTitle = "##ControllerCombo" + std::to_string(port);
	if (ImGui::BeginCombo(comboTitle.c_str(), SDL_GameControllerName(curController)))
	{
		// allow for a "None" option which deselects a port
		bool isSelected = (curController == nullptr);
		if (ImGui::Selectable("None", isSelected))
			m_pControllerHandler->m_Ports.Disconnect(port);
		
		if (isSelected) ImGui::SetItemDefaultFocus();

		// iterate & list Controllers
		for (int i = 0; i < controllers.size(); ++i)
		{
			isSelected = (controllers.at(i) == curController);
			if (ImGui::Selectable(SDL_GameControllerName(controllers.at(i)), isSelected))
				m_pControllerHandler->m_Ports.Connect(controllers.at(i), port);

			if (isSelected) ImGui::SetItemDefaultFocus();
		}

		ImGui::EndCombo();
	}
}

/// @brief Renders the ImGui frame within the SDL2 context
void EmulatorDisplay::RenderImGuiFrame() 
{
	ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
}

/// @brief Cleans up GUI stuff
void EmulatorDisplay::ShutdownImGui() 
{
	ImGui_ImplSDLRenderer2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}

/// @brief Responsible for starting and rendering ImGui and SDL2 frames
/// @param screenBuffer Texture raw data
/// @param size Size of each row of data
void EmulatorDisplay::RenderFrame(BYTE* screenBuffer, int size)
{
	SDL_RenderClear(GetRenderer());
	
	SDL_UpdateTexture(GetTexture(), NULL, screenBuffer, size * 3);
	SDL_RenderCopy(GetRenderer(), GetTexture(), NULL, NULL);
	StartImGuiFrame();
	RenderImGuiFrame();

	SDL_RenderPresent(GetRenderer());
}



std::string EmulatorDisplay::GetSelectedFile()
{
	std::lock_guard<std::mutex> lock(fileStrMutex);
	return m_selectedFile;
};

void EmulatorDisplay::SetSelectedFile(const std::string& selectedFile)
{
	std::lock_guard<std::mutex> lock(fileStrMutex);
	m_selectedFile = selectedFile;
}