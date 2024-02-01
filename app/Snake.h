#include "../core/memory/RAM.h"
#include "SDL/SDLApp.h"

#define UP_KEY      0x77
#define DOWN_KEY    0x73
#define LEFT_KEY    0x61
#define RIGHT_KEY   0x64

#define RNG_POS     0xFE
#define DIR_POS     0xFF

#define SCREEN_START 0x0200
#define SCREEN_END   0x0600

#define SIZE 32
#define SCALE 10

class Snake
{
    RAM* m_RAM{};
    std::unique_ptr<SDLApp> m_app = nullptr;
    uint8_t m_Screen[SIZE * SIZE * 3]{};
    SDL_Event m_Event{};

    void HandleInput(SDL_Event &e);
    SDL_Color GetColor(uint8_t byte);
    bool ReadScreen(BYTE* frame);

public:
    Snake(RAM* ram);

    void Run();
};