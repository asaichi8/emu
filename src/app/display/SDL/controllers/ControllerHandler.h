#pragma once

#include <SDL2/SDL.h>
#include <vector>
#include "Ports.h"


class ControllerHandler
{
	std::vector<SDL_GameController*> m_Controllers{};

	std::vector<SDL_GameController*> RetrieveControllers();

public:
    ControllerHandler();
    ~ControllerHandler();

	Ports m_Ports;

	void UpdateControllers();
    const std::vector<SDL_GameController*>& GetControllers() const { return m_Controllers; }
};