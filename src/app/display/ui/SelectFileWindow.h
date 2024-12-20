#pragma once

#include <vector>
#include <atomic>
#include "IGUIWindow.h"


class SelectFileWindow : public IGUIWindow
{
    std::atomic<bool> m_atomicIsOpen{};

	// TODO: implement this
	// void OpenFileDialog(std::atomic<bool>* pShouldCPURun)
	// {
	// 	this->Open(true);
	// 	// pause CPU while we're opening a file - we still need to render though
	// 	bool preservedShouldCPURun = *pShouldCPURun;
	// 	*pShouldCPURun = false; // pause cpu

	// 	const char* filterPatterns[] = {"*.nes"};
	// 	const char* filePath = tinyfd_openFileDialog("Select a file", "", 1, filterPatterns, "NES files", 0);

	// 	if (filePath)
	// 	{
	// 		SetSelectedFile(filePath);
	// 		m_recentFiles.Push(filePath);
	// 	}
		
	// 	*pShouldCPURun = preservedShouldCPURun;
	// 	this->Open(false);
	// }

public:
    SelectFileWindow(const std::string& name) : IGUIWindow(name) {}

    void Open(bool open) override { m_atomicIsOpen.store(open); }
    bool IsOpen() const override { return m_atomicIsOpen.load(); }

    void Draw() override
    {
		ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		
		static const char* title = "Select a file";
		ImGui::OpenPopup(title);
		if (ImGui::BeginPopupModal(title, nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Please choose a file.");
		}

		ImGui::EndPopup();
    }
};