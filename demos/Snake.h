#pragma once

#include <SDL2/SDL.h>
#include <memory>
#include "Bus.h"

#define UP_KEY      0x77
#define DOWN_KEY    0x73
#define LEFT_KEY    0x61
#define RIGHT_KEY   0x64

#define RNG_POS     0xFE
#define DIR_POS     0xFF

#define SCREEN_START 0x0200
#define SCREEN_END   0x0600


class Snake
{
    std::shared_ptr<Bus> m_Bus{};
    SDL_Event m_Event{};

    SDL_Color GetColor(BYTE byte);
    bool ReadScreen(BYTE* frame);

public:
    Snake(std::shared_ptr<Bus> bus);
    ~Snake();

    bool Run(BYTE* m_Screen);
    void HandleEvent(const SDL_Event& e);
};