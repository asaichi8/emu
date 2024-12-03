#pragma once

#include <SDL2/SDL.h>
#include <memory>
#include "../core/memory/Bus.h"

#define UP_KEY      0x77
#define DOWN_KEY    0x73
#define LEFT_KEY    0x61
#define RIGHT_KEY   0x64

#define WIDTH 256
#define HEIGHT 240


class NESDisplay
{
	std::shared_ptr<Bus> m_Bus{};
	SDL_Event m_Event{};
    size_t m_szScreen[WIDTH][HEIGHT];

	SDL_Color GetColor(BYTE byte);
	bool ReadScreen(BYTE* frame);

public:
	NESDisplay(std::shared_ptr<Bus> bus);
	~NESDisplay();

	bool Run(BYTE* m_Screen);
	void HandleEvent(const SDL_Event& e);
};
