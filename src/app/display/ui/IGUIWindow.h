#pragma once

#include "../../../include/imgui/imgui.h"


class IGUIWindow
{

public:
    virtual ~IGUIWindow() = default;

    bool m_isOpen{};

    virtual void Draw() = 0;
};