#include "EmulatorDisplay.h"

EmulatorDisplay::EmulatorDisplay(const std::string& winName, int w, int h, int scale, ControllerHandler* pCH) 
	: SDLApp(winName, w, h, scale)
{
	this->InitImGui();

	m_mainMenuBar = std::make_unique<MainMenuBar>(m_uiManager, pCH);
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

	m_mainMenuBar->Draw();
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