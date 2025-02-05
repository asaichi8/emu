#include "CPURegWindow.h"

CPURegWindow::CPURegWindow(const std::string& name, CPURegisters& CPUReg) : IGUIWindow(name), m_CPUReg(CPUReg)
{

}

void CPURegWindow::Draw()
{

    ImGui::Begin("CPU", &this->m_isOpen, ImGuiWindowFlags_AlwaysAutoResize);
    
    ImGui::Text("PC: 0x%04X", m_CPUReg.program_counter);
    ImGui::Text("SP: 0x%02X", m_CPUReg.stack_pointer);
    ImGui::Text("A:  0x%02X", m_CPUReg.accumulator);
    ImGui::Text("X:  0x%02X", m_CPUReg.X);
    ImGui::Text("Y:  0x%02X", m_CPUReg.Y);
    ImGui::Separator();
    // set status register flag to green if toggled, red if otherwise
    ImGui::TextColored(m_CPUReg.status_register.test(StatusRegisterFlags::CARRY) ? GREEN : RED, "C"); ImGui::SameLine();
    ImGui::TextColored(m_CPUReg.status_register.test(StatusRegisterFlags::ZERO) ? GREEN : RED, "Z"); ImGui::SameLine();
    ImGui::TextColored(m_CPUReg.status_register.test(StatusRegisterFlags::INTERRUPT_REQUEST) ? GREEN : RED, "I"); ImGui::SameLine();
    ImGui::TextColored(m_CPUReg.status_register.test(StatusRegisterFlags::DECIMAL_MODE) ? GREEN : RED, "D"); ImGui::SameLine();
    ImGui::TextColored(m_CPUReg.status_register.test(StatusRegisterFlags::BREAK_COMMAND) ? GREEN : RED, "B"); ImGui::SameLine();
    ImGui::TextColored(m_CPUReg.status_register.test(StatusRegisterFlags::UNUSED) ? GREEN : RED, "U"); ImGui::SameLine();
    ImGui::TextColored(m_CPUReg.status_register.test(StatusRegisterFlags::OVERFLOW_FLAG) ? GREEN : RED, "O"); ImGui::SameLine();
    ImGui::TextColored(m_CPUReg.status_register.test(StatusRegisterFlags::NEGATIVE) ? GREEN : RED, "N");
    
    ImGui::End();
}