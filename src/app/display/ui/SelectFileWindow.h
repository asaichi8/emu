#pragma once

#include <vector>
#include "IGUIWindow.h"


class SelectFileWindow : public IGUIWindow
{

public:
    SelectFileWindow(const std::string& name) : IGUIWindow(name) {}

    void Draw() override
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
};