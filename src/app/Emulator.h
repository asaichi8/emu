#pragma once

#include <memory>
#include <chrono>
#include <thread>
#include <stdexcept>
#include <unordered_map>
#include "../core/loader/ROM.h"
#include "../core/memory/Bus.h"
#include "../core/cpu/CPU.h"
#include "../core/ppu/PPU.h"
#include "display/EmulatorDisplay.h"
#include "display/NESDisplay.h"

//constexpr char ROM_RELATIVE_PATH[] = "../bin/donkeykong.nes"; 
constexpr char PALETTE_RELATIVE_PATH[] = "../bin/ntscpalette.pal";  // TODO: add this to args or preload binary or something


/// @brief Responsible for the execution of all the emulator components.
class Emulator
{
	int m_FPS = 60; // FPS of the GUI

	ROM m_ROM{};
	std::unique_ptr<Palette> m_pPalette{};
	std::shared_ptr<Bus> m_Bus{};
	std::unique_ptr<CPU> m_CPU{};
	EmulatorDisplay* m_GUI{};

	const static inline std::unordered_map<SDL_KeyCode, Joypad::Button> m_buttonMap = {
		{SDLK_w, Joypad::Button::UP},
		{SDLK_s, Joypad::Button::DOWN},
		{SDLK_a, Joypad::Button::LEFT},
		{SDLK_d, Joypad::Button::RIGHT},
		{SDLK_j, Joypad::Button::A},
		{SDLK_k, Joypad::Button::B},
		{SDLK_x, Joypad::Button::START},
		{SDLK_z, Joypad::Button::SELECT}
	};

public:
	Emulator(const std::string& romPath, EmulatorDisplay& GUI);
	~Emulator();

	bool Run();
};
