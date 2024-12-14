#pragma once

#include <SDL2/SDL.h>
#include <stdexcept>
#include "Config.h"


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

    bool SaveToConfig();

    SDL_JoystickID GetJoystickID(size_t port);
    SDL_JoystickGUID GetJoystickGUID(size_t port);
    size_t GetPortSize() { return m_noPorts; }
};