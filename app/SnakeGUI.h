#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include "SDLApp.h"

class SnakeGUI : public SDLApp
{
    bool shouldCPURun = true;
    bool shouldRestart = false;
    bool shouldStepThrough = false;

    void StartImGuiFrame();
    void RenderImGuiFrame();

public:
    SnakeGUI(int w, int h, int scale);

    void InitImGui();
    void RenderFrame(uint8_t* screenBuffer, int size);
    void ShutdownImGui();

    bool GetShouldCPURun() { return shouldCPURun; }
    bool GetShouldRestart() { return shouldRestart; }
    void SetShouldRestart(bool b) { shouldRestart = b; }

    bool GetShouldStepThrough() { return shouldStepThrough; }
    void SetShouldStepThrough(bool b) { shouldStepThrough = b; }
};