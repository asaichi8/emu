#include "AddCodeWindow.h"


AddCodeWindow::AddCodeWindow(const std::string& name) : IGUIWindow(name)
{

}


void AddCodeWindow::Draw()
{
    ImGui::SetNextWindowSizeConstraints({0.f, 0.f}, {750.f, 500.f});
    ImGui::Begin("Add Game Genie Code", &this->m_isOpen, ImGuiWindowFlags_AlwaysAutoResize);
    
    // IM_ARRAYSIZE - " Don't use on pointers!", so pass it here instead
    CreateEntry("Address:", m_szAddress, IM_ARRAYSIZE(m_szAddress));
    CreateEntry("Value:  ", m_szValue, IM_ARRAYSIZE(m_szValue));
    CreateEntry("Compare:", m_szCompare, IM_ARRAYSIZE(m_szCompare));

    ImGui::End();
}


void AddCodeWindow::CreateEntry(const std::string& entryName, char* buffer, size_t bufSize)
{
    ImGui::Text("%s 0x", entryName.c_str());
    ImGui::SameLine();

    std::string textboxName = "##" + entryName;
    ImGui::InputText(textboxName.c_str(), buffer, bufSize, ImGuiInputTextFlags_CharsHexadecimal);
}