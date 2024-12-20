#pragma once

#include <vector>
#include "IGUIWindow.h"


class ErrorMsgWindow : public IGUIWindow
{
    std::string m_lastErrorTitle = "Error";
    std::string m_lastError = "Unknown error occured!";

public:
    ErrorMsgWindow(const std::string& name)
        : IGUIWindow(name) {}

    void Draw() override
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
				this->Open(false);
			}

		}
		
		ImGui::EndPopup();
		ImGui::PopStyleColor();
    }

    void SetError(const std::string& title, const std::string& error)
    {
        m_lastErrorTitle = title;
        m_lastError = error;
    }
};