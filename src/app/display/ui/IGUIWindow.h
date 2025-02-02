#pragma once

#include <string>
#include "../../../include/lib/imgui/imgui.h"


/// @brief Interface for creating an ImGUI window. 
class IGUIWindow
{
protected:
    std::string m_name{}; // name of the window (used to search for a window internally)
    bool m_isOpen{}; // whether we should render the window or not

public:
    IGUIWindow(const std::string& name) : m_name(name) {}
    virtual ~IGUIWindow() = default;

    virtual void Draw() = 0;
    virtual void Open(bool shouldOpen) { m_isOpen = shouldOpen; } // set m_isOpen
    virtual bool IsOpen() const { return m_isOpen; }
    
    const std::string& GetName() const { return m_name; }
};