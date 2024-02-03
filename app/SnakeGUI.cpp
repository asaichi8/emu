#include "SnakeGUI.h"


SnakeGUI::SnakeGUI(int w, int h, int scale) : SDLApp("Snake", w, h, scale)
{
    
}


void SnakeGUI::InitImGui() 
{
    if (!m_Window || !m_Renderer)       
        throw std::runtime_error("no window or renderer");

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui_ImplSDL2_InitForSDLRenderer(
        m_Window,
        m_Renderer
    );
    ImGui_ImplSDLRenderer2_Init(m_Renderer);
}

void SnakeGUI::StartImGuiFrame() 
{
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame(m_Window);

    ImGui::NewFrame();

    if (ImGui::BeginMainMenuBar()) 
    {
        if (ImGui::BeginMenu("File")) 
        {
            if (ImGui::MenuItem("Restart")) 
            {
                shouldRestart = true;
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Debug")) 
        {
            if (ImGui::MenuItem(shouldCPURun ? "Pause" : "Resume")) 
            {
                shouldCPURun = !shouldCPURun;
            }
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    ImGui::Render();
}

void SnakeGUI::RenderImGuiFrame() 
{
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
}

void SnakeGUI::ShutdownImGui() 
{
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}

void SnakeGUI::RenderFrame(uint8_t* screenBuffer, int size)
{
    SDL_RenderClear(GetRenderer());
    
    SDL_UpdateTexture(GetTexture(), NULL, screenBuffer, size * 3);
    SDL_RenderCopy(GetRenderer(), GetTexture(), NULL, NULL);
    StartImGuiFrame();
    RenderImGuiFrame();

    SDL_RenderPresent(GetRenderer());
}