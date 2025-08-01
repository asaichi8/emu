#pragma once

#include <SDL2/SDL.h>
#include <vector>
#include "Ports.h"


/// @brief Holds the current internal controller state (what controllers are connected to what ports), and handles saving/loading of controllers.
class ControllerHandler
{
	static const inline std::string STR_PORTS = "ports";
	std::vector<SDL_GameController*> m_Controllers{};

	std::vector<SDL_GameController*> RetrieveControllers();

public:
    ControllerHandler();
    ~ControllerHandler();

	Ports m_Ports;

	void UpdateControllers();
    bool SaveToConfig();
    bool LoadFromConfig();

    const std::vector<SDL_GameController*>& GetControllers() const { return m_Controllers; }
};