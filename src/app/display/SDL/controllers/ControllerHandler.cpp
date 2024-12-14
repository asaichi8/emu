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
	LoadFromConfig();
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

bool ControllerHandler::SaveToConfig()
{
    Config* config = &Config::GetInstance();

    bool failedToRead = config->_file.read(config->ini);
	
    char szGuid[33];
    for (int i = 1; i <= m_Ports.GetPortSize(); ++i)
    {
        SDL_JoystickGetGUIDString(m_Ports.GetJoystickGUID(i), szGuid, sizeof(szGuid));
        std::string strGuid = szGuid; // convert to string for ease of use
        // null devices are set to all zeros, so we check if that's the case, and prefer an empty string
        if (std::all_of(strGuid.begin(), strGuid.end(), [](char c) { return c == '0'; }))
            strGuid = "";

        config->ini[STR_PORTS]["port" + std::to_string(i)] = strGuid;
    }

    bool writeSuccess{};
    if (failedToRead)
        writeSuccess = config->_file.generate(config->ini, true);
    else
        writeSuccess = config->_file.write(config->ini, true);

    return writeSuccess;
}

bool ControllerHandler::LoadFromConfig()
{
    Config* config = &Config::GetInstance();

    if (!config->_file.read(config->ini) || !config->ini.has(STR_PORTS))
        return false;
    
	int curPort = 0;
	for (const auto& [key, val] : config->ini[STR_PORTS])
	{
		curPort++;
        if (val.empty() || m_Ports.GetPortSize() < curPort)
            continue;

		const std::string& savedVal = config->ini[STR_PORTS]["port" + std::to_string(curPort)];

		for (auto& controller : m_Controllers)
		{
			if (!controller)
				continue;
			
			SDL_Joystick* joystick = SDL_GameControllerGetJoystick(controller);
			if (!joystick)
				continue;

			char szGuid[33];
			SDL_JoystickGetGUIDString(SDL_JoystickGetGUID(joystick), szGuid, sizeof(szGuid));

			if (std::string(szGuid) != savedVal)
				continue;
			
			m_Ports.Connect(controller, curPort);
			break;
		}
	}

	return true;
}