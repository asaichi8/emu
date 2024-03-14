#pragma once

#include <memory>
#include <chrono>
#include <thread>
#include <stdexcept>
#include "../core/loader/ROM.h"
#include "../core/memory/Bus.h"
#include "../core/cpu/CPU.h"
#include "../core/ppu/PPU.h"
#include "display/EmulatorDisplay.h"
#include "../include/Path.h"
#include "../demos/Snake.h"

#define SIZE 32  // Size of each internal pixel
#define SCALE 20 // Amount to scale the internal pixels up by


/// @brief Responsible for the execution of all the emulator components.
class Emulator
{
    static const int SCREEN_BUFFER_SIZE = SIZE * SIZE * 3; // [Snake] Size of the buffer required for the Snake game.
    int m_FPS = 60; // FPS of the GUI

    ROM m_ROM{};
    std::shared_ptr<Bus> m_Bus{};
    std::unique_ptr<CPU> m_CPU{};
    std::unique_ptr<PPU> m_PPU{};
    std::unique_ptr<Snake> m_Snake{};
    std::unique_ptr<EmulatorDisplay> m_GUI{};

public:
    Emulator();
    ~Emulator();

    void Run();
};
