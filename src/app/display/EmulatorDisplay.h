#pragma once

#include <atomic>
#include <thread>
#include <mutex>
#include <iostream>
#include "SDL/SDLApp.h"
#include "../../include/tinyfiledialogs/tinyfiledialogs.h"
#include "../../include/imgui/imgui.h"
#include "../../include/imgui/imgui_impl_sdl2.h"
#include "../../include/imgui/imgui_impl_sdlrenderer2.h"
#include "../../core/cpu/CPURegisters.h"
#include "../../include/typedefs.h"
#include "../../core/loader/Loader.h"
#include "SDL/controllers/ControllerHandler.h"
#include "../../include/Config.h"
#include "RecentFileQueue.h"
#include "ui/UIManager.h"
#include "ui/GameGenieWindow.h"


/// @brief Responsible for the GUI/video of the emulator.
class EmulatorDisplay : public SDLApp
{
	ControllerHandler* m_pControllerHandler;
	CPURegisters m_curReg{}; // A copy of the CPU's registers so they can be displayed.
	std::vector<GameGenie::GameGenieCode>* m_pCodes;
	std::string m_lastError = "Unknown error occured!";
	std::string m_lastErrorTitle = "Error";
	std::string m_selectedFile{};
	UIManager m_uiManager{};

	// TODO: this should probably be some kind of array
	std::atomic<bool> shouldCPURun = true;
	bool shouldRestart = false;
	bool shouldStepThrough = false;
	bool shouldReadRegisters = false;
	bool shouldShowErrorMsg = false;
	std::atomic<bool> shouldShowFileDialog = false;
	bool shouldOpenControllerWin = false;
	bool shouldOpenGameGenieWin = false;

	void StartImGuiFrame();
	void RenderImGuiFrame();
	void OpenFileDialog();
	void CreateControllerCombo(size_t port);

public:
	EmulatorDisplay(const std::string& winName, int w, int h, int scale, ControllerHandler* pCH);
	~EmulatorDisplay();

	std::mutex fileStrMutex{};
	RecentFileQueue m_recentFiles;

	void InitImGui();
	void RenderFrame(BYTE* screenBuffer, int size);
	void ShutdownImGui();
	void UpdateRegisters(const CPURegisters& registers) { m_curReg = registers; }
	void UpdateCodes(std::vector<GameGenie::GameGenieCode>* codes) { m_pCodes = codes; }


	// Getters/setters
	bool GetShouldCPURun() const { return shouldCPURun.load(); }

	bool GetShouldRestart() { return shouldRestart; }
	void SetShouldRestart(bool b) { shouldRestart = b; }

	bool GetShouldStepThrough() { return shouldStepThrough; }
	void SetShouldStepThrough(bool b) { shouldStepThrough = b; }

	bool GetShouldReadRegisters() { return shouldReadRegisters; }
	void SetShouldReadRegisters(bool b) { shouldReadRegisters = b; }

	bool GetShouldShowErrorMsg() { return shouldShowErrorMsg; }
	void SetShouldShowErrorMsg(bool b, const std::string& errMsg = "Unknown error occured!", const std::string& errTitle = "Error")
	{ 
		shouldShowErrorMsg = b;
		m_lastError = errMsg; 
		m_lastErrorTitle = errTitle;
	}

	std::string GetSelectedFile();
	void SetSelectedFile(const std::string& selectedFile);

	bool GetShouldShowFileDialog() const { return shouldShowFileDialog.load(); }
	ControllerHandler* GetControllerHandler() { return m_pControllerHandler; }
};
