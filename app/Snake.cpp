#include "Snake.h"

Snake::Snake(RAM* ram) : m_app(std::make_unique<SDLApp>("Snake", SIZE, SIZE, SCALE)),
    m_RAM(ram)
{
    m_app->InitImGui();
}

Snake::~Snake()
{
    m_app->ShutdownImGui();
}

bool Snake::Run()
{
    HandleInput(m_Event);

    if (!m_app->GetShouldCPURun())
    {
        RenderScreen();
        return m_app->GetShouldCPURun(); // always false
    }

    m_RAM->WriteByte(RNG_POS, (BYTE)(rand() % 14) + 1);

    if (!ReadScreen(m_Screen))
        return true;
        
    RenderScreen();

    return m_app->GetShouldCPURun();
}


void Snake::HandleInput(SDL_Event &e) 
{
    while (SDL_PollEvent(&e)) 
    {
        ImGui_ImplSDL2_ProcessEvent(&e);
        if (e.type == SDL_KEYDOWN && m_app->GetShouldCPURun()) 
        {
            switch (e.key.keysym.sym) 
            {
                case SDLK_w:
                    m_RAM->WriteByte(DIR_POS, UP_KEY);
                    break;
                case SDLK_s:
                    m_RAM->WriteByte(DIR_POS, DOWN_KEY);
                    break;
                case SDLK_a:
                    m_RAM->WriteByte(DIR_POS, LEFT_KEY);
                    break;
                case SDLK_d:
                    m_RAM->WriteByte(DIR_POS, RIGHT_KEY);
                    break;
                default:
                    break;
            }
        }
        else if (e.type == SDL_QUIT) 
            std::exit(0);
    }
}

SDL_Color Snake::GetColor(uint8_t byte) 
{
    switch (byte) 
    {
        case 0: return SDL_Color{0, 0, 0, 255};       // BLACK
        case 1: return SDL_Color{255, 255, 255, 255}; // WHITE
        case 2:
        case 9: return SDL_Color{128, 128, 128, 255}; // GREY
        case 3:
        case 10: return SDL_Color{255, 0, 0, 255};    // RED
        case 4:
        case 11: return SDL_Color{0, 255, 0, 255};    // GREEN
        case 5:
        case 12: return SDL_Color{0, 0, 255, 255};    // BLUE
        case 6:
        case 13: return SDL_Color{255, 0, 255, 255};  // MAGENTA
        case 7:
        case 14: return SDL_Color{255, 255, 0, 255};  // YELLOW
        default: return SDL_Color{0, 255, 255, 255};  // CYAN
    }
}

bool Snake::ReadScreen(BYTE* szFrame)
{
    bool shouldUpdate = false;
    int frame_i = 0;

    for (WORD addr = SCREEN_START; addr < SCREEN_END; ++addr) 
    {
        SDL_Color color = GetColor(m_RAM->ReadByte(addr));

        if (szFrame[frame_i] != color.r || szFrame[frame_i + 1] != color.g || szFrame[frame_i + 2] != color.b) 
        {
            szFrame[frame_i] = color.r;
            szFrame[frame_i + 1] = color.g;
            szFrame[frame_i + 2] = color.b;

            shouldUpdate = true;
        }

        frame_i += 3;
    }

    return shouldUpdate;
}

void Snake::RenderScreen()
{
    SDL_RenderClear(m_app->GetRenderer());
    
    SDL_UpdateTexture(m_app->GetTexture(), NULL, m_Screen, SIZE * 3);
    SDL_RenderCopy(m_app->GetRenderer(), m_app->GetTexture(), NULL, NULL);
    m_app->StartImGuiFrame();
    m_app->RenderImGuiFrame();

    SDL_RenderPresent(m_app->GetRenderer());
}