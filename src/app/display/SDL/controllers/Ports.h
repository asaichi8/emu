#pragma once

#include <SDL2/SDL.h>
#include <stdexcept>


class Ports
{
    size_t m_noPorts{};
	SDL_GameController** m_ConnectedControllers{};

public:
    Ports(size_t noPorts);
    ~Ports();

    bool Connect(SDL_GameController*& controller, size_t port);
    void Disconnect(size_t port);
	SDL_GameController*& Retrieve(size_t port) const;
    SDL_JoystickID GetJoystickID(size_t port) { return SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(this->Retrieve(port))); }
};