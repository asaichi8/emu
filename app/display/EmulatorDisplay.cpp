#include "EmulatorDisplay.h"


EmulatorDisplay::EmulatorDisplay(const std::string& winName, int w, int h, int scale) 
    : SDLApp(winName, w, h, scale)
{
    
}


/// @brief Initialise the ImGui window and integrate it with the SDL2 renderer
void EmulatorDisplay::InitImGui() 
{
    if (!m_Window || !m_Renderer)       
        throw std::runtime_error("no window or renderer");

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.IniFilename = NULL;
    io.LogFilename = NULL;

    ImGui_ImplSDL2_InitForSDLRenderer(
        m_Window,
        m_Renderer
    );
    ImGui_ImplSDLRenderer2_Init(m_Renderer);
}

/// @brief Called every frame that the GUI is rendered - consists of the actual UI
void EmulatorDisplay::StartImGuiFrame() 
{
    // Begin a new frame for our window
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
            if (ImGui::MenuItem(shouldReadRegisters ? "Hide registers" : "Display registers"))
            {
                shouldReadRegisters = !shouldReadRegisters;
            }
            if (!shouldCPURun)
            {
                if (ImGui::MenuItem("Step through")) 
                {
                    shouldStepThrough = true;
                }
            }
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    if (shouldReadRegisters)
    {
        static const ImVec4 green = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
        static const ImVec4 red   = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);

        ImGui::Begin("CPU", &shouldReadRegisters, ImGuiWindowFlags_AlwaysAutoResize);
        
        ImGui::Text("PC: 0x%04X", m_curReg.program_counter);
        ImGui::Text("SP: 0x%02X", m_curReg.stack_pointer);
        ImGui::Text("A:  0x%02X", m_curReg.accumulator);
        ImGui::Text("X:  0x%02X", m_curReg.X);
        ImGui::Text("Y:  0x%02X", m_curReg.Y);
        ImGui::Separator();
        ImGui::TextColored(m_curReg.status_register.test(StatusRegisterFlags::CARRY) ? green : red, "C"); ImGui::SameLine();
        ImGui::TextColored(m_curReg.status_register.test(StatusRegisterFlags::ZERO) ? green : red, "Z"); ImGui::SameLine();
        ImGui::TextColored(m_curReg.status_register.test(StatusRegisterFlags::INTERRUPT_REQUEST) ? green : red, "I"); ImGui::SameLine();
        ImGui::TextColored(m_curReg.status_register.test(StatusRegisterFlags::DECIMAL_MODE) ? green : red, "D"); ImGui::SameLine();
        ImGui::TextColored(m_curReg.status_register.test(StatusRegisterFlags::BREAK_COMMAND) ? green : red, "B"); ImGui::SameLine();
        ImGui::TextColored(m_curReg.status_register.test(StatusRegisterFlags::UNUSED) ? green : red, "U"); ImGui::SameLine();
        ImGui::TextColored(m_curReg.status_register.test(StatusRegisterFlags::OVERFLOW) ? green : red, "O"); ImGui::SameLine();
        ImGui::TextColored(m_curReg.status_register.test(StatusRegisterFlags::NEGATIVE) ? green : red, "N");
        
        ImGui::End();
            
    }

    ImGui::Render();
}

/// @brief Renders the ImGui frame within the SDL2 context
void EmulatorDisplay::RenderImGuiFrame() 
{
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
}

/// @brief Cleans up GUI stuff
void EmulatorDisplay::ShutdownImGui() 
{
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}

/// @brief Responsible for starting and rendering ImGui and SDL2 frames
/// @param screenBuffer Texture raw data
/// @param size Size of each row of data
void EmulatorDisplay::RenderFrame(BYTE* screenBuffer, int size)
{
    SDL_RenderClear(GetRenderer());
    
    SDL_UpdateTexture(GetTexture(), NULL, screenBuffer, size * 3);
    SDL_RenderCopy(GetRenderer(), GetTexture(), NULL, NULL);
    StartImGuiFrame();
    RenderImGuiFrame();

    SDL_RenderPresent(GetRenderer());
}