#pragma once

#include "../core/ppu/PPU.h"
#include "../core/loader/Palette.h"

#define DISPLAY_WIDTH 256
#define DISPLAY_HEIGHT 240

class NESDisplay
{
	struct Point
	{
		int32_t x{};
		int32_t y{};

		Point operator+(const Point& r) const { return { this->x + r.x, this->y + r.y }; }
		Point operator-(const Point& r) const { return { this->x - r.x, this->y - r.y }; }
		Point operator-(int i) 			const { return { this->x - i, this->y - i }; }
		Point operator*(int i) 			const { return { this->x * i, this->y * i }; }
	};

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
	BYTE m_szScreenBuffer[4][SCREEN_BUFFER_SIZE]{};

	bool SetPixel(const RGB colour, const Point& pixelPos, RGB transparentColour, bool behindBg, size_t bufNum = 0);
	void DrawTile(const Tile &tile, const Point& tilePos, const std::vector<BYTE> &tilePalette, size_t bufNum = 0,
				  const Point& start = {0, 0}, const Point& end = {DISPLAY_WIDTH, DISPLAY_HEIGHT}, const Point& shift = {0, 0},
				  bool isSprite = false, bool flipY = false, bool flipX = false, bool behindBg = false);
	std::vector<BYTE> GetBgTilePalette(const std::vector<BYTE> &nametable, const Point& tileNo);
	std::vector<BYTE> GetSpriteTilePalette(const std::bitset<2>& paletteIndex);
	void DrawNametable(const std::vector<BYTE>& nametable, size_t bufNum = 0, const Point& start = {0, 0}, const Point& end = {DISPLAY_WIDTH, DISPLAY_HEIGHT}, const Point& shift = {0, 0});
	void DrawSprites();

public:
	NESDisplay(const PPU *pPPU, Palette *pPalette);
	~NESDisplay();

	void DrawTiles(const std::vector<BYTE> *pCHR_ROM, const size_t bank, size_t bufNum = 0, const Point& startPos = {0, 0}, size_t lineLength = 16);
	void DrawScreen();
	BYTE* GetScreen(size_t bufNum)
	{
		if (bufNum >= 4)
			return 0;
		
		return m_szScreenBuffer[bufNum];
	}
};
