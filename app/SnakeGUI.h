#pragma once

#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include "SDLApp.h"
#include "CPURegisters.h"

class SnakeGUI : public SDLApp
{
    bool shouldCPURun = true;
    bool shouldRestart = false;
    bool shouldStepThrough = false;
    bool shouldReadRegisters = false;

    CPURegisters m_curRegisters{};

    void StartImGuiFrame();
    void RenderImGuiFrame();

public:
    SnakeGUI(int w, int h, int scale);

    void InitImGui();
    void RenderFrame(uint8_t* screenBuffer, int size);
    void ShutdownImGui();
    void UpdateRegisters(const CPURegisters& registers) {
        m_curRegisters = registers;
    }


    bool GetShouldCPURun() { return shouldCPURun; }

    bool GetShouldRestart() { return shouldRestart; }
    void SetShouldRestart(bool b) { shouldRestart = b; }

    bool GetShouldStepThrough() { return shouldStepThrough; }
    void SetShouldStepThrough(bool b) { shouldStepThrough = b; }

    bool GetShouldReadRegisters() { return shouldReadRegisters; }
    void SetShouldReadRegisters(bool b) { shouldReadRegisters = b; }
};