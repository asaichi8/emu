#pragma once

#include <vector>
#include "pch.h"
#include "IGUIWindow.h"
#include "../SDL/controllers/ControllerHandler.h"


/// @brief Responsible for creating the window in which controllers are configured from. Holds and modifies the controller handler.
class ControllerWindow : public IGUIWindow
{
    ControllerHandler* m_pControllerHandler{};

    void CreateControllerCombo(size_t port);

public:
    ControllerWindow(const std::string& name, ControllerHandler* pControllerHandler);

    void Draw() override;
};