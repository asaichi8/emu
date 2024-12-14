#include "Ports.h"


Ports::Ports(size_t noPorts) : m_noPorts(noPorts), m_ConnectedControllers(new SDL_GameController*[noPorts]())
{
    for (size_t i = 0; i < noPorts; ++i)
        m_ConnectedControllers[i] = nullptr;
}

Ports::~Ports()
{
    delete[] m_ConnectedControllers;
}


// return false on failure
bool Ports::Connect(SDL_GameController*& controller, size_t port)
{
    if (port == 0 || port > m_noPorts)
        return false;

    m_ConnectedControllers[port - 1] = controller;
    return true;
}

void Ports::Disconnect(size_t port)
{
    m_ConnectedControllers[port - 1] = nullptr;
}

bool Ports::SaveToConfig()
{
    Config* config = &Config::GetInstance();

    bool failedToRead = config->_file.read(config->ini);

    char szGuid[33];
    for (int i = 1; i <= m_noPorts; ++i)
    {
        SDL_JoystickGetGUIDString(GetJoystickGUID(i), szGuid, sizeof(szGuid));
        config->ini["ports"]["port" + std::to_string(i)] = szGuid;
    }

    bool writeSuccess{};
    if (failedToRead)
        writeSuccess = config->_file.generate(config->ini, true);
    else
        writeSuccess = config->_file.write(config->ini, true);

    return writeSuccess;
}

SDL_GameController*& Ports::Retrieve(size_t port) const
{ 
    if (port == 0 || port > m_noPorts) 
        throw std::out_of_range("Invalid port number");

    return m_ConnectedControllers[port - 1];
}

SDL_JoystickID Ports::GetJoystickID(size_t port) 
{
    if (Retrieve(port) == nullptr)
        return -1;

    return SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(this->Retrieve(port)));
}
SDL_JoystickGUID Ports::GetJoystickGUID(size_t port)
{
    if (Retrieve(port) == nullptr)
        return {};
    
    SDL_GameController* controller = Retrieve(port);
    if (!controller)
        return {};

    SDL_Joystick* joystick = SDL_GameControllerGetJoystick(controller);
    if (!joystick)
        return {};
    
    return SDL_JoystickGetGUID(joystick);
}