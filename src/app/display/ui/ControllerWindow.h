#pragma once

#include <vector>
#include <iostream>
#include "IGUIWindow.h"
#include "../SDL/controllers/ControllerHandler.h"


class ControllerWindow : public IGUIWindow
{
    ControllerHandler* m_pControllerHandler{};

    void CreateControllerCombo(size_t port)
    {
        ImGui::Text("Port %d: ", port);
        ImGui::SameLine();

        auto curController = m_pControllerHandler->m_Ports.Retrieve(port);

        // check if current controller is still connected
        // (we do this here because if it's disconnected, it can't write anything anyway)
        auto controllers = m_pControllerHandler->GetControllers();
        if (curController != nullptr && std::find(controllers.begin(), controllers.end(), curController) == controllers.end())
        {
            // not connected so disconnect
            m_pControllerHandler->m_Ports.Disconnect(port);
            curController = nullptr;
        }

        std::string comboTitle = "##ControllerCombo" + std::to_string(port);
        if (ImGui::BeginCombo(comboTitle.c_str(), SDL_GameControllerName(curController)))
        {
            // allow for a "None" option which deselects a port
            bool isSelected = (curController == nullptr);
            if (ImGui::Selectable("None", isSelected))
                m_pControllerHandler->m_Ports.Disconnect(port);
            
            if (isSelected) ImGui::SetItemDefaultFocus();

            // iterate & list Controllers
            for (int i = 0; i < controllers.size(); ++i)
            {
                isSelected = (controllers.at(i) == curController);
                if (ImGui::Selectable(SDL_GameControllerName(controllers.at(i)), isSelected))
                    m_pControllerHandler->m_Ports.Connect(controllers.at(i), port);

                if (isSelected) ImGui::SetItemDefaultFocus();
            }

            ImGui::EndCombo();
        }
    }

public:
    ControllerWindow(const std::string& name, ControllerHandler* pControllerHandler) : IGUIWindow(name), m_pControllerHandler(pControllerHandler) {}

    void Draw() override
    {
        if (!m_pControllerHandler)
            return;

		ImGui::Begin("Controller", &this->m_isOpen, ImGuiWindowFlags_AlwaysAutoResize);

		// create combo for each port
		for (int i = 1; i <= m_pControllerHandler->m_Ports.GetPortSize(); ++i)
		{
			CreateControllerCombo(i);
		}

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		if (ImGui::Button("Save defaults", ImVec2(ImGui::GetContentRegionAvail().x, 0)))
		{
			if (!m_pControllerHandler->SaveToConfig())
				std::cerr << "Failed to save ports!" << std::endl;
		}

		ImGui::End();
    }
};