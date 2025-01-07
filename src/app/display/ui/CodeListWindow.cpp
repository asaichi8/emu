#include "CodeListWindow.h"


CodeListWindow::CodeListWindow(const std::string& name, Loader::GameInfo** pGameInfo) : IGUIWindow(name), m_ppGameInfo(pGameInfo)
{

}


void CodeListWindow::Draw()
{
    if (!m_ppGameInfo || !*m_ppGameInfo)
        return;

    auto pGameInfo = *m_ppGameInfo;
        
    // TODO: set description to right hand side
    // TODO: add "add code"
    ImGui::SetNextWindowSizeConstraints({300.f, 200.f}, {750.f, 500.f});
    ImGui::Begin("Game Genie Codes", &this->m_isOpen, ImGuiWindowFlags_HorizontalScrollbar);
    
    if (pGameInfo->gameGenieCodes.empty())
    {
        ImGui::Text("No codes are currently loaded.");
        ImGui::End();
        return;
    }
    
    ImGui::BeginChild("Code List", {0, -ImGui::GetFrameHeightWithSpacing()}, false, ImGuiWindowFlags_AlwaysVerticalScrollbar);
    for (int i = 0; i < pGameInfo->gameGenieCodes.size(); ++i) // loop through each code
    {
        GameGenie::GameGenieCode* code = &pGameInfo->gameGenieCodes.at(i);

        ImGui::PushID(i); // give unique ID to each checkbox

        ImGui::Checkbox("##EnableCode", &code->isActive);
            // pGameInfo->BuildCodeMap();

        ImGui::SameLine();
        for (int i = 0; i < code->code.size(); ++i)
        {
            ImGui::Text("%s", code->code.at(i).c_str());
            if (ImGui::IsItemHovered())
            {
                ImGui::BeginTooltip();
                ImGui::Text("Address = 0x%x", code->decoded.at(i).addr);
                ImGui::Text("Value = 0x%x", code->decoded.at(i).val);
                if (code->decoded.at(i).compare.has_value())
                    ImGui::Text("Compare = 0x%x", code->decoded.at(i).compare.value());
                ImGui::EndTooltip();
            }

            if (i != code->code.size() - 1) // if not last element
            {
                ImGui::SameLine();
                ImGui::Text("+");
            }

            ImGui::SameLine();
        }
        ImGui::Text(":  %s", code->description.c_str());
        
        //ImGui::SameLine();
        //ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(code->description.c_str()).x); // set to right hand side
        //ImGui::Text("%s", code->description.c_str());

        ImGui::PopID();
    }


    ImGui::EndChild();

    if (ImGui::Button("Save changes (unimplemented)", {ImGui::GetContentRegionAvail().x, 0}))
    {
        // TODO: implement me
    }

    ImGui::End();
}