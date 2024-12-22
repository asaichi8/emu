#include "GameGenieWindow.h"


GameGenieWindow::GameGenieWindow(const std::string& name, Loader::GameInfo** pGameInfo) : IGUIWindow(name), m_ppGameInfo(pGameInfo)
{

}


void GameGenieWindow::Draw()
{
    if (!m_ppGameInfo || !*m_ppGameInfo)
        return;

    auto pGameInfo = *m_ppGameInfo;
        
    // TODO: resize this window based on the length, and set description to right hand side
    // TODO: add "add code"
    ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    ImGui::Begin("Game Genie Codes", &this->m_isOpen, ImGuiWindowFlags_AlwaysAutoResize);
    
    for (int i = 0; i < pGameInfo->gameGenieCodes.size(); ++i) // loop through each code
    {
        GameGenie::GameGenieCode* code = &pGameInfo->gameGenieCodes.at(i);

        ImGui::PushID(i); // give unique ID to each checkbox

        if (ImGui::Checkbox("##EnableCode", &code->isActive))
            pGameInfo->BuildCodeMap();

        ImGui::SameLine();
        ImGui::Text("%s :  %s", code->code.c_str(), code->description.c_str());
        //ImGui::SameLine();
        //ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(code->description.c_str()).x); // set to right hand side
        //ImGui::Text("%s", code->description.c_str());

        ImGui::PopID();
    }

    ImGui::End();
}