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
#include "display/NESDisplay.h"

constexpr char ROM_RELATIVE_PATH[] = "../../../bin/myrom.nes"; 
constexpr char PALETTE_RELATIVE_PATH[] = "../../../bin/ntscpalette.pal"; 


/// @brief Responsible for the execution of all the emulator components.
class Emulator
{
	int m_FPS = 60; // FPS of the GUI

	ROM m_ROM{};
	std::unique_ptr<Palette> m_pPalette{};
	std::shared_ptr<Bus> m_Bus{};
	std::unique_ptr<CPU> m_CPU{};
	std::unique_ptr<EmulatorDisplay> m_GUI{};

public:
	Emulator();
	~Emulator();

	void Run();
};
