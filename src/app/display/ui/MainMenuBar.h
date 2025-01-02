#pragma once

#include <thread>
#include <atomic>
#include <mutex>
#include "UIManager.h"
#include "CodeListWindow.h"
#include "ControllerWindow.h"
#include "CPURegWindow.h"
#include "SelectFileWindow.h"
#include "ErrorMsgWindow.h"
#include "AddCodeWindow.h"
#include "../ConfigQueue.h"
#include "../../../include/tinyfiledialogs/tinyfiledialogs.h"


class MainMenuBar
{
    UIManager& m_uiManager;
    ControllerHandler* m_pControllerHandler;
	CPURegisters m_curReg{}; // A copy of the CPU's registers so they can be displayed.
	Loader::GameInfo* m_pGameInfo;
	std::string m_selectedFile{};

	std::atomic<bool> m_bShouldCPURun = true;
	bool m_bShouldRestart = false;
	bool m_bShouldStepThrough = false;

    void OpenFileDialog(std::atomic<bool>* pShouldCPURun);


public:
    MainMenuBar(UIManager& uiManager, ControllerHandler* pCH);

    ConfigQueue m_recentFiles;
	std::mutex fileStrMutex{};

    void Draw();


	void UpdateRegisters(const CPURegisters& registers) { m_curReg = registers; }
	void UpdateCodes(Loader::GameInfo* info) { m_pGameInfo = info; }
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