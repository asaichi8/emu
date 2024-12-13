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

SDL_GameController*& Ports::Retrieve(size_t port) const
{ 
    if (port == 0 || port > m_noPorts) 
        throw std::out_of_range("Invalid port number");

    return m_ConnectedControllers[port - 1];
}