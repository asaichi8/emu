#include "AddCodeWindow.h"


AddCodeWindow::AddCodeWindow(const std::string& name) : IGUIWindow(name)
{

}


void AddCodeWindow::Draw()
{
    ImGui::SetNextWindowSizeConstraints({0.f, 0.f}, {750.f, 500.f});
    ImGui::Begin("Add Game Genie Code", &this->m_isOpen, ImGuiWindowFlags_AlwaysAutoResize);
    
    // IM_ARRAYSIZE - "Don't use on pointers!", so pass it here instead
    CreateEntry("Address: 0x", m_szAddress, IM_ARRAYSIZE(m_szAddress), ImGuiInputTextFlags_CharsHexadecimal);
    CreateEntry("Value:   0x", m_szValue,   IM_ARRAYSIZE(m_szValue)  , ImGuiInputTextFlags_CharsHexadecimal);
    CreateEntry("Compare: 0x", m_szCompare, IM_ARRAYSIZE(m_szCompare), ImGuiInputTextFlags_CharsHexadecimal);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    CreateEntry("Code:      ", m_szCode, IM_ARRAYSIZE(m_szCode));

    ImGui::Spacing();

    if (ImGui::Button("Add to code list", ImVec2(ImGui::GetContentRegionAvail().x, 0)))
    {
        // TODO: implement add to code list
    }

    ImGui::End();
}


void AddCodeWindow::CreateEntry(const std::string& entryName, char* buffer, size_t bufSize, ImGuiInputTextFlags flags)
{
    ImGui::Text("%s", entryName.c_str());
    ImGui::SameLine();

    std::string textboxName = "##" + entryName;
    ImGui::InputText(textboxName.c_str(), buffer, bufSize, flags);
}