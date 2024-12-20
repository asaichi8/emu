#pragma once

#include <vector>
#include <memory>
#include <unordered_map>
#include "IGUIWindow.h"


// mediator pattern
class UIManager
{
    // keep a duplicate name for efficiency
    std::unordered_map<std::string, std::shared_ptr<IGUIWindow>> m_Windows;

public:
    //UIManager() {}

    void DrawAll()
    {
        for (auto& [name, window] : m_Windows)
        {
            if (window->IsOpen())
                window->Draw();
        }
    }

    std::shared_ptr<IGUIWindow> GetWindow(const std::string& name)
    {
        auto it = m_Windows.find(name);
        if (it != m_Windows.end())
            return it->second;

        return nullptr;
    }

    void RegisterWindow(std::shared_ptr<IGUIWindow> window) { m_Windows[window->GetName()] = window; }
};