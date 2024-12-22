#include "EmulatorDisplay.h"

// TODO: refactor this its a mess
EmulatorDisplay::EmulatorDisplay(const std::string& winName, int w, int h, int scale, ControllerHandler* pCH) 
	: SDLApp(winName, w, h, scale), m_recentFiles("recentfiles", 10), m_pControllerHandler(pCH) 
{
	this->InitImGui();

	auto gameGenieWindow  = std::make_shared<GameGenieWindow> ("Game Genie",  &m_pCodes); // pass reference as m_pCodes may change
	auto controllerWindow = std::make_shared<ControllerWindow>("Controllers", pCH);
	auto cpuRegWindow     = std::make_shared<CPURegWindow>	  ("Registers", m_curReg);
	auto selectFileWindow = std::make_shared<SelectFileWindow>("Load file");
	auto errorMsgWindow   = std::make_shared<ErrorMsgWindow>  ("Show error");
	
	m_uiManager.RegisterWindow(gameGenieWindow);
	m_uiManager.RegisterWindow(controllerWindow);
	m_uiManager.RegisterWindow(cpuRegWindow);
	m_uiManager.RegisterWindow(selectFileWindow);
	m_uiManager.RegisterWindow(errorMsgWindow);
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

/// @brief Called every frame that the GUI is rendered - consists of the actual UI
void EmulatorDisplay::StartImGuiFrame() 
{
	// Begin a new frame for our window
	ImGui_ImplSDLRenderer2_NewFrame();
	ImGui_ImplSDL2_NewFrame(m_Window);

	ImGui::NewFrame();

	DrawMainMenu();
	m_uiManager.DrawAll();

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


void EmulatorDisplay::DrawMainMenu()
{
	if (ImGui::BeginMainMenuBar()) 
	{
		if (ImGui::BeginMenu("File")) 
		{
			if (ImGui::MenuItem("Load file"))
			{
				std::thread t_FileDialog([this]() { OpenFileDialog(&m_bShouldCPURun); });
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
				m_bShouldRestart = true;

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Debug")) 
		{
			if (ImGui::MenuItem(m_bShouldCPURun ? "Pause" : "Resume")) 
				m_bShouldCPURun = !m_bShouldCPURun;
				
			if (auto win = m_uiManager.GetWindow("Registers"); win && ImGui::MenuItem(win->IsOpen() ? "Hide registers" : "Display registers"))
				win->Open(!win->IsOpen()); // toggle
				
			if (!m_bShouldCPURun && ImGui::MenuItem("Step through")) 
				m_bShouldStepThrough = true;
					
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Settings"))
		{
			if (auto win = m_uiManager.GetWindow("Controllers"); win && ImGui::MenuItem(win->GetName().c_str()))
				win->Open(true);
			
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Cheats"))
		{
			if (auto win = m_uiManager.GetWindow("Game Genie"); win && ImGui::MenuItem(win->GetName().c_str()))
				win->Open(true);
			
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}

void EmulatorDisplay::OpenFileDialog(std::atomic<bool>* pShouldCPURun)
{
	m_uiManager.GetWindow("Load file")->Open(true);
	// pause CPU while we're opening a file - we still need to render though
	bool preservedShouldCPURun = *pShouldCPURun;
	*pShouldCPURun = false; // pause cpu

	const char* filterPatterns[] = {"*.nes"};
	const char* filePath = tinyfd_openFileDialog("Select a file", "", 1, filterPatterns, "NES files", 0);

	if (filePath)
	{
		SetSelectedFile(filePath);
		m_recentFiles.Push(filePath);
	}
	
	*pShouldCPURun = preservedShouldCPURun;
	m_uiManager.GetWindow("Load file")->Open(false);
}