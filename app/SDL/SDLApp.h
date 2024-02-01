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

public:
    SDLApp(const std::string& windowName, int w, int h, int scale);
    ~SDLApp();

    SDL_Window* GetWindow();
    SDL_Renderer* GetRenderer();
    SDL_Texture* GetTexture();
};