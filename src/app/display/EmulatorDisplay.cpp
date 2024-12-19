#include "EmulatorDisplay.h"

// TODO: refactor this its a mess
EmulatorDisplay::EmulatorDisplay(const std::string& winName, int w, int h, int scale, ControllerHandler* pCH) 
	: SDLApp(winName, w, h, scale), m_pControllerHandler(pCH)
{
	this->InitImGui();

	auto gameGenieWindow  = std::make_shared<GameGenieWindow> ("Game Genie",  &m_pCodes); // pass reference as m_pCodes may change
	auto controllerWindow = std::make_shared<ControllerWindow>("Controllers", pCH);
	auto cpuRegWindow     = std::make_shared<CPURegWindow>("Registers", m_curReg);
	
	m_uiManager.RegisterWindow(gameGenieWindow);
	m_uiManager.RegisterWindow(controllerWindow);
	m_uiManager.RegisterWindow(cpuRegWindow);
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
	ImGuiIO& io = ImGui::GetIO(); //(void)io;
	//io.IniFilename = NULL;
	//io.LogFilename = NULL;

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
				
			if (auto win = m_uiManager.GetWindow("Registers"); win && ImGui::MenuItem(win->m_isOpen ? "Hide registers" : "Display registers"))
				win->m_isOpen = !win->m_isOpen;
				
			if (!shouldCPURun && ImGui::MenuItem("Step through")) 
				shouldStepThrough = true;
					
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Settings"))
		{
			if (auto win = m_uiManager.GetWindow("Controllers"); win && ImGui::MenuItem(win->GetName().c_str()))
				win->m_isOpen = true;
			
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Cheats"))
		{
			if (auto win = m_uiManager.GetWindow("Game Genie"); win && ImGui::MenuItem(win->GetName().c_str()))
				win->m_isOpen = true;
			
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	m_uiManager.DrawAll();

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