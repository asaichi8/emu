#pragma once

#include "RAM.h"
#include <SDL2/SDL.h>

#define UP_KEY      0x77
#define DOWN_KEY    0x73
#define LEFT_KEY    0x61
#define RIGHT_KEY   0x64

#define RNG_POS     0xFE
#define DIR_POS     0xFF

#define SCREEN_START 0x0200
#define SCREEN_END   0x0600

#define SIZE 32
#define SCALE 20

class Snake
{
    RAM* m_RAM{};
    SDL_Event m_Event{};

    SDL_Color GetColor(uint8_t byte);
    bool ReadScreen(BYTE* frame);

public:
    Snake(RAM* ram);
    ~Snake();

    bool Run(BYTE* m_Screen);
    void HandleEvent(const SDL_Event& e);
};