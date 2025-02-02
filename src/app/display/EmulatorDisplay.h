#pragma once

#include <atomic>
#include <thread>
#include <mutex>
#include "pch.h"
#include "SDL/SDLApp.h"
#include "../../include/lib/imgui/imgui.h"
#include "../../include/lib/imgui/imgui_impl_sdl2.h"
#include "../../include/lib/imgui/imgui_impl_sdlrenderer2.h"
#include "../../include/typedefs.h"
#include "../../core/loader/Loader.h"
#include "SDL/controllers/ControllerHandler.h"
#include "ui/UIManager.h"
#include "ui/MainMenuBar.h"


/// @brief Responsible for the GUI/video of the emulator.
class EmulatorDisplay : public SDLApp
{
	UIManager m_uiManager{};
	std::unique_ptr<MainMenuBar> m_mainMenuBar;

	void StartImGuiFrame();
	void RenderImGuiFrame();

public:
	EmulatorDisplay(const std::string& winName, int w, int h, int scale, ControllerHandler* pCH);
	~EmulatorDisplay();

	void InitImGui();
	void RenderFrame(BYTE* screenBuffer, int size);
	void ShutdownImGui();

	std::unique_ptr<MainMenuBar>& GetMainMenuBar() { return m_mainMenuBar; }
};
