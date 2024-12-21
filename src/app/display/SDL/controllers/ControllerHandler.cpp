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


/// @brief Called when we need to update the internal controller state (e.g. on program start, plugging in/disconnecting controllers etc.)
void ControllerHandler::UpdateControllers()
{
    m_Controllers = RetrieveControllers();
	LoadFromConfig(); // check if we plugged in a default controller
}

/// @brief Saves the current internal controller state to an INI file, so we can remember the user's currently connected controllers when
///		   RAM is cleared (e.g. "save as default"). If a controller is empty, its GUID is set to an empty string.
/// @return Returns true if we successfully wrote to the file, false if otherwise.
bool ControllerHandler::SaveToConfig()
{
    Config* config = &Config::GetInstance();

    bool failedToRead = config->_file.read(config->ini);
	
    char szGuid[33]; // GUIDs are 32 characters log
    for (int i = 1; i <= m_Ports.GetPortSize(); ++i)
    {
        SDL_JoystickGetGUIDString(m_Ports.GetJoystickGUID(i), szGuid, sizeof(szGuid));
        std::string strGuid = szGuid; // convert to string for ease of use
        // null devices are set to all zeros, so we check if that's the case, and prefer an empty string
        if (std::all_of(strGuid.begin(), strGuid.end(), [](char c) { return c == '0'; }))
            strGuid = "";

        config->ini[STR_PORTS]["port" + std::to_string(i)] = strGuid; // write GUID to port in config
    }

    bool writeSuccess{};
    if (failedToRead) // if we failed to read the file probably didn't exist, so try generating one
        writeSuccess = config->_file.generate(config->ini, true); 
    else // read successful, attempt to write to file
        writeSuccess = config->_file.write(config->ini, true);

    return writeSuccess;
}

/// @brief Loads the controller state from the previously saved config (saved using SaveToConfig())
/// @return Returns false if the file does not exist, and true on a successful load.
bool ControllerHandler::LoadFromConfig()
{
    Config* config = &Config::GetInstance();

	// check if we can read any ports from the config file
    if (!config->_file.read(config->ini) || !config->ini.has(STR_PORTS))
        return false;
    
	int curPort = 0;
	for (const auto& [key, val] : config->ini[STR_PORTS]) // for each port/GUID pair...
	{
		curPort++; // ports start at 1
		if (m_Ports.GetPortSize() < curPort)
			break; // too many keys!

        if (val.empty())
            continue;
		
		const std::string& savedVal = config->ini[STR_PORTS]["port" + std::to_string(curPort)];
		
		// we've retrieved the GUID - now we need to check if any connected controllers match it, so we can connect them
		// to the port
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
			
			m_Ports.Connect(controller, curPort); // matched default controller, so connect

			break; 
		}
	}

	return true;
}


/// @brief Retrieves a list of pointers to every currently connected controller.
/// @return A list of pointers to every connected controller.
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
