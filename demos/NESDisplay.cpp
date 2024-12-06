#include "NESDisplay.h"
// TODO: add sprite rendering

NESDisplay::NESDisplay(const PPU* pPPU, Palette* pPalette) : m_pPPU(pPPU), m_pPalette(pPalette)
{

}

NESDisplay::~NESDisplay()
{

}


void NESDisplay::DrawTiles(const std::vector<BYTE> *pCHR_ROM, const size_t bank)
{
	const WORD tilesStartAddr = bank * 0x1000;

	size_t tileScreenPosX = 0;
	size_t tileScreenPosY = 0;
	std::vector<Tile> tiles{};
	for (int tileNo = 0; tileNo < DISPLAY_WIDTH; ++tileNo)
	{
		const WORD curTileOffset = tileNo * sizeof(Tile);
		const WORD curTilePos = tilesStartAddr + curTileOffset;

		if (tileNo && tileNo % 16 == 0)
		{
			tileScreenPosY += 8; // new line
			tileScreenPosX = 0;	 // reset x at end of line
		}

		// map 16 bytes at curTilePos into curTile
		const Tile *pCurTile = (const Tile *)(&(pCHR_ROM->at(curTilePos)));
		tiles.push_back(*pCurTile); // push_back makes a copy, so no problem dereferencing a temporary pointer

		DrawTile(*pCurTile, tileScreenPosX, tileScreenPosY);

		tileScreenPosX += 8;
	}
}

void NESDisplay::DrawNametable()
{
	const WORD bgBankAddr = dynamic_cast<PPUCTRL *>(m_pPPU->registers.ppuctrl.get())->GetBackgroundPTableAddr();

	std::vector<Tile> tiles{};
	// NES screen is 32 tiles long, 30 tiles high (32 * 8 = 256, 30 * 8 = 240)
	for (int tileNo = 0; tileNo < 32 * 30; ++tileNo)
	{
		// bgBankAddr either 0 or 0x1000, use it to determine which bank we access
		const std::vector<BYTE> nametable = m_pPPU->GetNametableRAM()[bgBankAddr ? 1 : 0];

		if (tileNo >= nametable.size())
		{
			std::cerr << "Attempted to access tile outside of nametable!" << std::endl;
			break;
		}

		const WORD selectedTilePos = nametable.at(tileNo);				// position of tile to be used
		const WORD selectedTileOffset = selectedTilePos * sizeof(Tile); // address of tile to be used
		const Tile *pCurTile = (const Tile *)(&(m_pPPU->GetCHR_ROM()->at(bgBankAddr + selectedTileOffset)));

		const size_t screenPosX = tileNo % 32;
		const size_t screenPosY = tileNo / 32;

		DrawTile(*pCurTile, screenPosX * 8, screenPosY * 8);
	}
}


bool NESDisplay::SetPixel(const RGB colour, const size_t x, const size_t y)
{
	const size_t pixelStartPos = (x * 3) + (y * 3 * DISPLAY_WIDTH);
	if (pixelStartPos + sizeof(RGB) > SCREEN_BUFFER_SIZE)
		return false;

	m_szScreenBuffer[pixelStartPos] = colour.r;
	m_szScreenBuffer[pixelStartPos + 1] = colour.g;
	m_szScreenBuffer[pixelStartPos + 2] = colour.b;

	return true;
}

void NESDisplay::DrawTile(const Tile &tile, const size_t tileScreenPosX, const size_t tileScreenPosY)
{
	for (int curByte = 0; curByte < 8; ++curByte)
	{
		std::bitset<8> left = tile.left[curByte];
		std::bitset<8> right = tile.right[curByte];

		for (int curBit = 0; curBit < 8; ++curBit) // left to right
		{
			std::bitset<4> pixelNibble = left[curBit] + right[curBit];

			RGB rgb;
			switch (pixelNibble.to_ulong())
			{
				case 0: rgb = m_pPalette->GetPalette().at(0x01); break;
				case 1: rgb = m_pPalette->GetPalette().at(0x10); break;
				case 2: rgb = m_pPalette->GetPalette().at(0x20); break;
				case 3: rgb = m_pPalette->GetPalette().at(0x30); break;
				default: std::cerr << "pixelNibble out of range - should never occur" << std::endl; break;
			}

			const size_t pixelPosX = (7 - curBit) + tileScreenPosX;
			const size_t pixelPosY = curByte + tileScreenPosY;

			if (!SetPixel(rgb, pixelPosX, pixelPosY))
				std::cerr << "Failed to draw pixel! (pixel out of bounds)" << std::endl;
		}
	}
}