#include "ErrorMsgWindow.h"


ErrorMsgWindow::ErrorMsgWindow(const std::string& name) : IGUIWindow(name) 
{

}


void ErrorMsgWindow::Draw()
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

/// @brief Store the error message in this class, presumably to display it next time we want to pop up an error message.
/// @param title Title of the error message.
/// @param error Content of the error message.
void ErrorMsgWindow::SetError(const std::string& title, const std::string& error)
{
    m_lastErrorTitle = title;
    m_lastError = error;
}