#include "AddCodeWindow.h"


AddCodeWindow::AddCodeWindow(const std::string& name) : IGUIWindow(name)
{

}


void AddCodeWindow::Draw()
{
    ImGui::SetNextWindowSizeConstraints({0.f, 0.f}, {750.f, 500.f});
    ImGui::Begin("Add Game Genie Code", &this->m_isOpen, ImGuiWindowFlags_AlwaysAutoResize);
    


    ImGui::End();
}