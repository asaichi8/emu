#include "UIManager.h"


/// @brief Draw every window in the list if it is open.
void UIManager::DrawAll()
{
    for (auto& [name, window] : m_Windows)
    {
        if (window->IsOpen())
            window->Draw();
    }
}

/// @brief Gets a pointer to a given window using the internal window unordered_map by its name.
/// @param name The name of the window to get.
/// @return A shared_ptr to the window we grabbed, or nullptr if we could not find the window.
std::shared_ptr<IGUIWindow> UIManager::GetWindow(const std::string& name)
{
    auto it = m_Windows.find(name);
    if (it != m_Windows.end())
        return it->second;

    return nullptr;
}

/// @brief Stores a given window in the unordered_map so we can get it later using GetWindow().
/// @param window A shared_ptr to the window to store.
void UIManager::RegisterWindow(std::shared_ptr<IGUIWindow> window)
{
    m_Windows[window->GetName()] = window;
}