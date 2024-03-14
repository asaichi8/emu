#pragma once

#include "../../include/imgui/imgui.h"
#include "../../include/imgui/imgui_impl_sdl2.h"
#include "../../include/imgui/imgui_impl_sdlrenderer2.h"
#include "SDL/SDLApp.h"
#include "../../core/cpu/CPURegisters.h"
#include "../../include/typedefs.h"


/// @brief Responsible for the GUI/video of the emulator.
class EmulatorDisplay : public SDLApp
{
    CPURegisters m_curReg{}; // A copy of the CPU's registers so they can be displayed.

    bool shouldCPURun = true;
    bool shouldRestart = false;
    bool shouldStepThrough = false;
    bool shouldReadRegisters = false;

    void StartImGuiFrame();
    void RenderImGuiFrame();

public:
    EmulatorDisplay(const std::string& winName, int w, int h, int scale);

    void InitImGui();
    void RenderFrame(BYTE* screenBuffer, int size);
    void ShutdownImGui();
    void UpdateRegisters(const CPURegisters& registers) { m_curReg = registers; }


    // Getters/setters
    bool GetShouldCPURun() { return shouldCPURun; }

    bool GetShouldRestart() { return shouldRestart; }
    void SetShouldRestart(bool b) { shouldRestart = b; }

    bool GetShouldStepThrough() { return shouldStepThrough; }
    void SetShouldStepThrough(bool b) { shouldStepThrough = b; }

    bool GetShouldReadRegisters() { return shouldReadRegisters; }
    void SetShouldReadRegisters(bool b) { shouldReadRegisters = b; }
};
