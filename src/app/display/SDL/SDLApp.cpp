#include "SDLApp.h"


SDLApp::SDLApp(const std::string& windowName, int w, int h, int scale) 
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0) 
		throw std::runtime_error("Failed to init SDL");
	
	SetupWindow(windowName, w, h, scale);
	SetupRenderer();
	SetupTexture(w, h);
}

SDLApp::~SDLApp()
{
	if (m_Texture)  SDL_DestroyTexture(m_Texture);
	if (m_Renderer) SDL_DestroyRenderer(m_Renderer);
	if (m_Window)   SDL_DestroyWindow(m_Window);

	SDL_Quit();
}


/// @brief Initialise the SDL window.
/// @param windowName Title of the window
/// @param w Raw width of the window
/// @param h Raw height of the window
/// @param scale Scale to multiply width/height by
void SDLApp::SetupWindow(const std::string& windowName, int w, int h, int scale)
{
	m_Window = SDL_CreateWindow(
		windowName.c_str(),              
		SDL_WINDOWPOS_CENTERED,     
		SDL_WINDOWPOS_CENTERED,     
		w * scale, 
		h * scale,
		SDL_WINDOW_SHOWN  
	);

	if (!m_Window) 
		throw std::runtime_error(std::string("Failed to init window: ") + SDL_GetError());
}

/// @brief Initialise the SDL renderer.
void SDLApp::SetupRenderer()
{
	m_Renderer = SDL_CreateRenderer(m_Window, -1, SDL_RENDERER_ACCELERATED);
	
	if (!m_Renderer) 
		throw std::runtime_error(std::string("Failed to init renderer: ") + SDL_GetError());
}

/// @brief Initialise a texture for the renderer.
/// @param w Width of the texture
/// @param h Height of the texture
void SDLApp::SetupTexture(int w, int h)
{
	m_Texture = SDL_CreateTexture(m_Renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, w, h);

	if (!m_Texture) 
		throw std::runtime_error(std::string("Failed to init texture: ") + SDL_GetError());
}