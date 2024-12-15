#pragma once

#include "../core/ppu/PPU.h"
#include "../core/loader/Palette.h"

#define DISPLAY_WIDTH 256
#define DISPLAY_HEIGHT 240

class NESDisplay
{
#pragma pack(push, 1)
	struct Tile
	{
		BYTE left[8]{};
		BYTE right[8]{};
	};
#pragma pack(pop)

	static const size_t SCREEN_BUFFER_SIZE = DISPLAY_WIDTH * DISPLAY_HEIGHT * sizeof(RGB); // 256 * 240 * 3

	const PPU *m_pPPU;
	Palette *m_pPalette;
	BYTE m_szScreenBuffer[SCREEN_BUFFER_SIZE]{};

	bool SetPixel(const RGB colour, const size_t x, const size_t y);
	void DrawTile(const Tile &tile, size_t tilePosX, size_t tilePosY, const std::vector<BYTE> &tilePalette, bool isSprite = false, bool flipY = false, bool flipX = false);
	std::vector<BYTE> GetBgTilePalette(const std::vector<BYTE> &nametable, const size_t tileNoX, const size_t tileNoY);
	std::vector<BYTE> GetSpriteTilePalette(const std::bitset<2>& paletteIndex);
	void DrawNametable();
	void DrawSprites();

public:
	NESDisplay(const PPU *pPPU, Palette *pPalette);
	~NESDisplay();

	void DrawTiles(const std::vector<BYTE> *pCHR_ROM, const size_t bank);
	void DrawScreen();
	BYTE *GetScreen() { return m_szScreenBuffer; }
};
