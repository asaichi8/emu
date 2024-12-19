#pragma once

#include <vector>
#include <iostream>
#include "IGUIWindow.h"
#include "../../../core/cpu/CPURegisters.h"


class CPURegWindow : public IGUIWindow
{
	static const inline ImVec4 GREEN = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
	static const inline ImVec4 RED   = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);

    CPURegisters& m_CPUReg;

public:
    CPURegWindow(const std::string& name, CPURegisters& CPUReg) : IGUIWindow(name), m_CPUReg(CPUReg) {}

    void Draw() override
    {

		ImGui::Begin("CPU", &this->m_isOpen, ImGuiWindowFlags_AlwaysAutoResize);
		
		ImGui::Text("PC: 0x%04X", m_CPUReg.program_counter);
		ImGui::Text("SP: 0x%02X", m_CPUReg.stack_pointer);
		ImGui::Text("A:  0x%02X", m_CPUReg.accumulator);
		ImGui::Text("X:  0x%02X", m_CPUReg.X);
		ImGui::Text("Y:  0x%02X", m_CPUReg.Y);
		ImGui::Separator();
		ImGui::TextColored(m_CPUReg.status_register.test(StatusRegisterFlags::CARRY) ? GREEN : RED, "C"); ImGui::SameLine();
		ImGui::TextColored(m_CPUReg.status_register.test(StatusRegisterFlags::ZERO) ? GREEN : RED, "Z"); ImGui::SameLine();
		ImGui::TextColored(m_CPUReg.status_register.test(StatusRegisterFlags::INTERRUPT_REQUEST) ? GREEN : RED, "I"); ImGui::SameLine();
		ImGui::TextColored(m_CPUReg.status_register.test(StatusRegisterFlags::DECIMAL_MODE) ? GREEN : RED, "D"); ImGui::SameLine();
		ImGui::TextColored(m_CPUReg.status_register.test(StatusRegisterFlags::BREAK_COMMAND) ? GREEN : RED, "B"); ImGui::SameLine();
		ImGui::TextColored(m_CPUReg.status_register.test(StatusRegisterFlags::UNUSED) ? GREEN : RED, "U"); ImGui::SameLine();
		ImGui::TextColored(m_CPUReg.status_register.test(StatusRegisterFlags::_OVERFLOW) ? GREEN : RED, "O"); ImGui::SameLine();
		ImGui::TextColored(m_CPUReg.status_register.test(StatusRegisterFlags::NEGATIVE) ? GREEN : RED, "N");
		
		ImGui::End();
    }
};