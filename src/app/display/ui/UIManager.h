#pragma once

#include <vector>
#include <memory>
#include <unordered_map>
#include "IGUIWindow.h"


/// @brief Mediator that holds and draws a list of given windows.
class UIManager
{
    // keep a duplicate name for efficiency
    std::unordered_map<std::string, std::shared_ptr<IGUIWindow>> m_Windows;

public:
    void DrawAll();
    std::shared_ptr<IGUIWindow> GetWindow(const std::string& name);
    void RegisterWindow(std::shared_ptr<IGUIWindow> window);
};