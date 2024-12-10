#pragma once

#include "../core/ppu/PPU.h"
#include "../core/loader/Palette.h"

#define DISPLAY_WIDTH 256
#define DISPLAY_HEIGHT 240

class NESDisplay
{
	// https://www.nesdev.org/wiki/PPU_OAM#Byte_2
	enum OAMProperties : BYTE
	{
		PALETTE_LOW = 1 << 0, // https://www.nesdev.org/wiki/PPU_registers#Scrolling
		PALETTE_HIGH = 1 << 1,
		UNUSED1 = 1 << 2,
		UNUSED2 = 1 << 3,
		UNUSED3 = 1 << 4,
		PRIORITY = 1 << 5, // 0 = in front of background, 1 = behind background
		FLIP_HORIZONTALLY = 1 << 6,
		FLIP_VERTICALLY = 1 << 7
	};

#pragma pack(push, 1)
	struct SpriteData
	{
		BYTE tileY{};
		BYTE tileIndex{};
		OAMProperties tileProperties{};
		BYTE tileX{};
	};
#pragma pop

#pragma pack(push, 1)
	struct Tile
	{
		BYTE left[8]{};
		BYTE right[8]{};
	};
#pragma pop

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
