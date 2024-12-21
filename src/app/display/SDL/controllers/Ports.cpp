#include "Ports.h"


Ports::Ports(size_t noPorts) : m_noPorts(noPorts), m_ConnectedControllers(new SDL_GameController*[noPorts]())
{
    // ensure all connected controllers are nullptr on initialisation
    for (size_t i = 0; i < noPorts; ++i)
        m_ConnectedControllers[i] = nullptr;
}

Ports::~Ports()
{
    delete[] m_ConnectedControllers;
}


/// @brief Handles connecting a controller to a port.
/// @param controller A reference to the pointer of the controller we want to connect.
/// @param port The port number to connect the controller to (starts at 1)
/// @return False if function was used incorrectly.
bool Ports::Connect(SDL_GameController*& controller, size_t port)
{
    if (port == 0 || port > m_noPorts)
        return false;

    m_ConnectedControllers[port - 1] = controller;
    return true;
}

/// @brief Disconnects a controller from a port by setting the reference to nullptr.
/// @param port The port number to connect the controller to (starts at 1)
void Ports::Disconnect(size_t port)
{
    m_ConnectedControllers[port - 1] = nullptr;
}

/// @brief Get the controller at a certain port. Throws if port number is invalid.
/// @param port The port number to connect the controller to (starts at 1)
/// @return A reference to the pointer pointing to the controller at the given port.
SDL_GameController*& Ports::Retrieve(size_t port) const
{
    if (port == 0 || port > m_noPorts)
        throw std::out_of_range("Invalid port number");

    return m_ConnectedControllers[port - 1];
}


/// @brief Retrieve the joystick ID of a controller at a given port, which can be compared to identify the connected controller.
/// @param port The port number to connect the controller to (starts at 1)
/// @return The joystick ID of the controller at a given port, or -1 if there is nothing in the port.
SDL_JoystickID Ports::GetJoystickID(size_t port)
{
    if (Retrieve(port) == nullptr)
        return -1;

    return SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(this->Retrieve(port)));
}

/// @brief Retrieve the joystick GUID of a controller at a given port, which is a unique identifier used to identify a controller.
/// @param port The port number to connect the controller to (starts at 1)
/// @return Returns an empty SDL_JoystickGUID on failure, and the GUID of the controller at the port on success.
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
