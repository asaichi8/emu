#include "MainMenuBar.h"


MainMenuBar::MainMenuBar(UIManager& uiManager, ControllerHandler* pCH) 
	:  m_recentFiles("recentfiles", 10), m_uiManager(uiManager), m_pControllerHandler(pCH)
{
	auto codeListWindow   = std::make_shared<CodeListWindow>  ("Code list",  &m_pGameInfo); // pass reference as m_pGameInfo may change
	auto controllerWindow = std::make_shared<ControllerWindow>("Controllers", pCH);
	auto cpuRegWindow     = std::make_shared<CPURegWindow>	  ("Registers", m_curReg);
	auto selectFileWindow = std::make_shared<SelectFileWindow>("Load file");
	auto errorMsgWindow   = std::make_shared<ErrorMsgWindow>  ("Show error");
	auto addCodeWindow    = std::make_shared<AddCodeWindow>   ("Add code");
	
	m_uiManager.RegisterWindow(codeListWindow);
	m_uiManager.RegisterWindow(controllerWindow);
	m_uiManager.RegisterWindow(cpuRegWindow);
	m_uiManager.RegisterWindow(selectFileWindow);
	m_uiManager.RegisterWindow(errorMsgWindow);
	m_uiManager.RegisterWindow(addCodeWindow);
}


void MainMenuBar::Draw()
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
			if (ImGui::BeginMenu("Game Genie"))
			{
				if (auto win = m_uiManager.GetWindow("Code list"); win && ImGui::MenuItem(win->GetName().c_str()))
					win->Open(true);
				
				if (auto win = m_uiManager.GetWindow("Add code"); win && ImGui::MenuItem(win->GetName().c_str()))
					win->Open(true);
				
				ImGui::EndMenu();
			}
			
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}

void MainMenuBar::OpenFileDialog(std::atomic<bool>* pShouldCPURun)
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