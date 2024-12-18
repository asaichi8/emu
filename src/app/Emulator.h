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
#include "../core/cheats/GameGenie.h"
#include "display/EmulatorDisplay.h"
#include "display/NESDisplay.h"
#include "ControllerHandler.h"

//constexpr char ROM_RELATIVE_PATH[] = "../bin/donkeykong.nes"; 
constexpr char PALETTE_RELATIVE_PATH[] = "../bin/ntscpalette.pal";  // TODO: add this to args or preload binary or something
constexpr char DATABASE_RELATIVE_PATH[] = "../bin/rom_database.json";  // TODO: add this to args or preload binary or something


/// @brief Responsible for the execution of all the emulator components.
class Emulator
{
	int m_FPS = 60; // FPS of the GUI

	ROM m_ROM{};
	GameGenie m_gameGenie{};
	std::unique_ptr<Palette> m_pPalette{};
	std::shared_ptr<Bus> m_Bus{};
	std::unique_ptr<CPU> m_CPU{};
	EmulatorDisplay* m_GUI{};
	std::string m_curRomPath{};

	const static inline std::unordered_map<SDL_KeyCode, Joypad::Button> m_keyButtonMap = {
		{SDLK_w, Joypad::Button::UP},
		{SDLK_s, Joypad::Button::DOWN},
		{SDLK_a, Joypad::Button::LEFT},
		{SDLK_d, Joypad::Button::RIGHT},
		{SDLK_j, Joypad::Button::A},
		{SDLK_k, Joypad::Button::B},
		{SDLK_x, Joypad::Button::START},
		{SDLK_z, Joypad::Button::SELECT}
	};

	// TODO: can probably combine this with the class above
	const static inline std::unordered_map<SDL_GameControllerButton, Joypad::Button> m_controllerButtonMap = {
		{SDL_CONTROLLER_BUTTON_DPAD_UP, Joypad::Button::UP},
		{SDL_CONTROLLER_BUTTON_DPAD_DOWN, Joypad::Button::DOWN},
		{SDL_CONTROLLER_BUTTON_DPAD_LEFT, Joypad::Button::LEFT},
		{SDL_CONTROLLER_BUTTON_DPAD_RIGHT, Joypad::Button::RIGHT},
		{SDL_CONTROLLER_BUTTON_A, Joypad::Button::A},
		{SDL_CONTROLLER_BUTTON_B, Joypad::Button::B},
		{SDL_CONTROLLER_BUTTON_START, Joypad::Button::START},
		{SDL_CONTROLLER_BUTTON_BACK, Joypad::Button::SELECT}
	};

public:
	Emulator(const std::string& romPath, EmulatorDisplay& GUI);
	~Emulator();

	std::string Run();
};
