#pragma once

#include <SDL2/SDL.h>
#include <string>
#include <stdexcept>


/// @brief Responsible for setting up the SDL2 renderer.
class SDLApp
{
    void SetupWindow(const std::string& windowName, int w, int h, int scale);
    void SetupRenderer(int scale);
    void SetupTexture(int w, int h);

protected:
    SDL_Window* m_Window     = nullptr;
    SDL_Renderer* m_Renderer = nullptr;
    SDL_Texture* m_Texture   = nullptr;

public:
    SDLApp(const std::string& windowName, int w, int h, int scale);
    ~SDLApp();

    SDL_Window* GetWindow() { return m_Window; }
    SDL_Renderer* GetRenderer() { return m_Renderer; }
    SDL_Texture* GetTexture() { return m_Texture; }
};