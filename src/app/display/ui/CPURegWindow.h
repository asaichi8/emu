#pragma once

#include <vector>
#include "pch.h"
#include "IGUIWindow.h"
#include "../../../core/cpu/CPURegisters.h"


class CPURegWindow : public IGUIWindow
{
	static const inline ImVec4 GREEN = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
	static const inline ImVec4 RED   = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);

    CPURegisters& m_CPUReg;

public:
    CPURegWindow(const std::string& name, CPURegisters& CPUReg);

    void Draw() override;
};