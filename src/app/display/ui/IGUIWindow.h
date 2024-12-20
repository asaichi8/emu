#pragma once

#include <string>
#include "../../../include/imgui/imgui.h"


class IGUIWindow
{
protected:
    std::string m_name{};
    bool m_isOpen{};

public:
    IGUIWindow(const std::string& name) : m_name(name) {}
    virtual ~IGUIWindow() = default;

    virtual void Draw() = 0;
    virtual void Open(bool shouldOpen) { m_isOpen = shouldOpen; }
    virtual bool IsOpen() const { return m_isOpen; }
    
    const std::string& GetName() const { return m_name; }
};