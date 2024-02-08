#pragma once

#include <memory>
#include <chrono>
#include <thread>
#include <stdexcept>
#include "ROM.h"
#include "Bus.h"
#include "CPU.h"
#include "PPU.h"
#include "EmulatorDisplay.h"
#include "Path.h"
#include "Snake.h"

#define SIZE 32
#define SCALE 20


class Emulator
{
    static const int SCREEN_BUFFER_SIZE = SIZE * SIZE * 3;
    int m_FPS = 60;

    ROM m_ROM{};
    std::shared_ptr<Bus> m_Bus{};
    std::unique_ptr<CPU> m_CPU{};
    std::unique_ptr<Snake> m_Snake{};
    std::unique_ptr<EmulatorDisplay> m_GUI{};

public:
    Emulator();
    ~Emulator();

    void Run();
};