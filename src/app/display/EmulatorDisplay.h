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
#include "ui/ControllerWindow.h"
#include "ui/CPURegWindow.h"
#include "ui/SelectFileWindow.h"
#include "ui/ErrorMsgWindow.h"


/// @brief Responsible for the GUI/video of the emulator.
class EmulatorDisplay : public SDLApp
{
	// TODO: most of this stuff should probably be in their respective classes
	ControllerHandler* m_pControllerHandler;
	CPURegisters m_curReg{}; // A copy of the CPU's registers so they can be displayed.
	std::vector<GameGenie::GameGenieCode>* m_pCodes;
	std::string m_lastError = "Unknown error occured!";
	std::string m_lastErrorTitle = "Error";
	std::string m_selectedFile{};
	UIManager m_uiManager{};

	// TODO: this should probably be some kind of array
	std::atomic<bool> m_bShouldCPURun = true;
	bool m_bShouldRestart = false;
	bool m_bShouldStepThrough = false;

	void StartImGuiFrame();
	void RenderImGuiFrame();
	void OpenFileDialog(std::atomic<bool>* pShouldCPURun);
	void DrawMainMenu();

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
	bool GetShouldCPURun() const { return m_bShouldCPURun.load(); }

	bool GetShouldRestart() { return m_bShouldRestart; }
	void SetShouldRestart(bool b) { m_bShouldRestart = b; }

	bool GetShouldStepThrough() { return m_bShouldStepThrough; }
	void SetShouldStepThrough(bool b) { m_bShouldStepThrough = b; }

	bool GetShouldShowErrorMsg() { return m_uiManager.GetWindow("Show error")->IsOpen(); }
	void SetShouldShowErrorMsg(bool b, const std::string& errMsg = "Unknown error occured!", const std::string& errTitle = "Error")
	{ 
		// cast to ErrorMsgWindow to use its functions
		std::shared_ptr<ErrorMsgWindow> errorMsgWin = std::dynamic_pointer_cast<ErrorMsgWindow>(m_uiManager.GetWindow("Show error"));

		if (!errorMsgWin)
			return;

		errorMsgWin->Open(b);
		errorMsgWin->SetError(errTitle, errMsg);
	}

	std::string GetSelectedFile()
	{
		std::lock_guard<std::mutex> lock(fileStrMutex);
		return m_selectedFile;
	};
	void SetSelectedFile(const std::string& selectedFile)
	{
		std::lock_guard<std::mutex> lock(fileStrMutex);
		m_selectedFile = selectedFile;
	}

	bool GetShouldShowFileDialog() { return m_uiManager.GetWindow("Select file")->IsOpen(); }
	ControllerHandler* GetControllerHandler() { return m_pControllerHandler; }
};
