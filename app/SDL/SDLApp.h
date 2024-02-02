
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include <SDL2/SDL.h>
#include <string>
#include <stdexcept>

class SDLApp
{
    SDL_Window* m_Window     = nullptr;
    SDL_Renderer* m_Renderer = nullptr;
    SDL_Texture* m_Texture   = nullptr;

    void SetupWindow(const std::string& windowName, int w, int h, int scale);
    void SetupRenderer(int scale);
    void SetupTexture(int w, int h);

    bool shouldCPURun = 1;

public:
    SDLApp(const std::string& windowName, int w, int h, int scale);
    ~SDLApp();

    SDL_Window* GetWindow();
    SDL_Renderer* GetRenderer();
    SDL_Texture* GetTexture();

    void InitImGui();
    void StartImGuiFrame();
    void RenderImGuiFrame();
    void ShutdownImGui();

    bool GetShouldCPURun() { return shouldCPURun; }
};