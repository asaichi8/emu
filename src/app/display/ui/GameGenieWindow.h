#pragma once

#include <vector>
#include "IGUIWindow.h"
#include "../../../core/loader/Loader.h"


class GameGenieWindow : public IGUIWindow
{
    std::vector<GameGenie::GameGenieCode>** m_ppCodes{};

public:
    GameGenieWindow(std::vector<GameGenie::GameGenieCode>** pCodes) : m_ppCodes(pCodes) {}

    void Draw() override
    {
        if (!m_ppCodes || !*m_ppCodes)
            return;

        auto pCodes = *m_ppCodes;
            
        // TODO: resize this window based on the length, and set description to right hand side
        // TODO: add "add code"
        ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiCond_Always);
        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        ImGui::Begin("Game Genie Codes", &this->m_isOpen, ImGuiWindowFlags_AlwaysAutoResize);
        
        for (int i = 0; i < pCodes->size(); ++i) // loop through each code
        {
            GameGenie::GameGenieCode* code = &pCodes->at(i);

            ImGui::PushID(i); // give unique ID to each checkbox

            ImGui::Checkbox("##EnableCode", &code->isActive);

            ImGui::SameLine();
            ImGui::Text("%s :  %s", code->code.c_str(), code->description.c_str());
            //ImGui::SameLine();
            //ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(code->description.c_str()).x); // set to right hand side
            //ImGui::Text("%s", code->description.c_str());

            ImGui::PopID();
        }

        ImGui::End();
    }
};