#pragma once

#include <string>
#include "../../../include/imgui/imgui.h"


class IGUIWindow
{
protected:
    std::string m_name{};

public:
    IGUIWindow(const std::string& name) : m_name(name) {}
    virtual ~IGUIWindow() = default;

    bool m_isOpen{};

    virtual void Draw() = 0;
    const std::string& GetName() const { return m_name; }
};