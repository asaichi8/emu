#include "GameGenieWindow.h"


GameGenieWindow::GameGenieWindow(const std::string& name, Loader::GameInfo** pGameInfo) : IGUIWindow(name), m_ppGameInfo(pGameInfo)
{

}


void GameGenieWindow::Draw()
{
    if (!m_ppGameInfo || !*m_ppGameInfo)
        return;

    auto pGameInfo = *m_ppGameInfo;
        
    // TODO: set description to right hand side
    // TODO: add "add code"
    ImGui::SetNextWindowSizeConstraints({0.f, 0.f}, {750.f, 500.f});
    ImGui::Begin("Game Genie Codes", &this->m_isOpen, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_HorizontalScrollbar);
    
    if (pGameInfo->gameGenieCodes.empty())
    {
        ImGui::Text("No codes are currently loaded.");
    }
    else
    {
        for (int i = 0; i < pGameInfo->gameGenieCodes.size(); ++i) // loop through each code
        {
            GameGenie::GameGenieCode* code = &pGameInfo->gameGenieCodes.at(i);

            ImGui::PushID(i); // give unique ID to each checkbox

            if (ImGui::Checkbox("##EnableCode", &code->isActive))
                pGameInfo->BuildCodeMap();

            ImGui::SameLine();
            ImGui::Text("%s :  %s", code->code.c_str(), code->description.c_str());
            if (ImGui::IsItemHovered())
            {
                ImGui::BeginTooltip();
                ImGui::Text("Address = 0x%x", code->decoded.addr);
                ImGui::Text("Value = 0x%x", code->decoded.val);
                if (code->decoded.compare.has_value())
                    ImGui::Text("Compare = 0x%x", code->decoded.compare.value());
                ImGui::EndTooltip();
            }
            //ImGui::SameLine();
            //ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(code->description.c_str()).x); // set to right hand side
            //ImGui::Text("%s", code->description.c_str());

            ImGui::PopID();
        }
    }

    ImGui::End();
}