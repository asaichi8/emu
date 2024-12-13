#include "ControllerHandler.h"


ControllerHandler::ControllerHandler() : m_Ports(2)
{
	UpdateControllers();
}

ControllerHandler::~ControllerHandler()
{
	for (const auto& controller : m_Controllers)
	{
		SDL_GameControllerClose(controller);
	}
}

void ControllerHandler::UpdateControllers()
{
    m_Controllers = RetrieveControllers();
}

std::vector<SDL_GameController*> ControllerHandler::RetrieveControllers()
{
	std::vector<SDL_GameController*> controllers{};

	for (int i = 0; i < SDL_NumJoysticks(); ++i)
	{
		if (!SDL_IsGameController(i))
			continue;
			
		controllers.push_back(SDL_GameControllerOpen(i));
	}

	return controllers;
}