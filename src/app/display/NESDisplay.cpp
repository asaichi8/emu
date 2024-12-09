#include "NESDisplay.h"
// TODO: add sprite rendering

NESDisplay::NESDisplay(const PPU* pPPU, Palette* pPalette) : m_pPPU(pPPU), m_pPalette(pPalette)
{

}

NESDisplay::~NESDisplay()
{

}


void NESDisplay::DrawScreen()
{
	DrawNametable();
	DrawSprites();
}

void NESDisplay::DrawTiles(const std::vector<BYTE> *pCHR_ROM, const size_t bank)
{
	const WORD tilesStartAddr = bank * 0x1000;

	size_t tilePosX = 0;
	size_t tilePosY = 0;
	std::vector<Tile> tiles{};
	for (int tileNo = 0; tileNo < DISPLAY_WIDTH; ++tileNo)
	{
		const WORD curTileOffset = tileNo * sizeof(Tile);
		const WORD curTilePos = tilesStartAddr + curTileOffset;

		if (tileNo && tileNo % 16 == 0)
		{
			tilePosY += 8; // new line
			tilePosX = 0;	 // reset x at end of line
		}

		// map 16 bytes at curTilePos into curTile
		const Tile *pCurTile = (const Tile *)(&(pCHR_ROM->at(curTilePos)));
		tiles.push_back(*pCurTile); // push_back makes a copy, so no problem dereferencing a temporary pointer

		// TODO: add palette (std::vector<BYTE> tilePalette = GetBgTilePalette(nametable, screenPosX, screenPosY);)
		//DrawTile(*pCurTile, tilePosX, tilePosY);

		tilePosX += 8;
	}
}


bool NESDisplay::SetPixel(const RGB colour, const size_t x, const size_t y)
{
	const size_t pixelStartPos = (x * sizeof(RGB)) + (y * sizeof(RGB) * DISPLAY_WIDTH);
	if (pixelStartPos + sizeof(RGB) > SCREEN_BUFFER_SIZE)
		return false; // not fatal

	// directly map the RGB colour struct into memory
	*(RGB*)(&m_szScreenBuffer[pixelStartPos]) = colour;

	return true;
}

// where tilePosX = 0-32, tilePosY = 0-30
void NESDisplay::DrawTile(const Tile &tile, size_t tilePosX, size_t tilePosY, const std::vector<BYTE>& tilePaletteIndexes, bool isSprite, bool flipY, bool flipX)
{
	for (int curByte = 0; curByte < 8; ++curByte)
	{
		std::bitset<8> left = tile.left[curByte];
		std::bitset<8> right = tile.right[curByte];

		for (int curBit = 0; curBit < 8; ++curBit) // left to right
		{
			std::bitset<4> pixelNibble = left[curBit] + right[curBit];
			if (isSprite && pixelNibble.none()) // if pixelNibble is 0 and it's a sprite, this pixel is transparent so we can skip it
				continue;

			RGB rgb = m_pPalette->GetPalette().at(tilePaletteIndexes.at(pixelNibble.to_ulong()));

			size_t pixelOffsetX = flipX ? curBit : 7 - curBit; // tiles are flipped on the x axis by default
			size_t pixelOffsetY = flipY ? 7 - curByte : curByte;
			
			SetPixel(rgb, tilePosX + pixelOffsetX, tilePosY + pixelOffsetY);
		}
	}
}

// see PPU Palette RAM grid: https://www.nesdev.org/wiki/PPU_palettes#Palette_RAM

// for example: tileNoX is 12 and tileNoY is 10, meaning the attribute table position is x == 3, y == 2
// 
//  o: tile
//  b: byte
//	X: targetted tile
//  O: tiles that the attribute table are targetting
//  B: targetted attribute table member (byte)
//
// 		nametable layout (32x30 grid of tiles): 							attribute table (8x8 grid of bytes):
// 		o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o		b b b b b b b b
// 		o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o		b b b b b b b b
// 		o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o		b b b B b b b b
// 		o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o		b b b b b b b b
// 		o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o		b b b b b b b b
// 		o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o		b b b b b b b b
// 		o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o		b b b b b b b b
// 		o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o		b b b b b b b b
// 		o o o o o o o o o o o o O O O O o o o o o o o o o o o o o o o o		
// 		o o o o o o o o o o o o O O O O o o o o o o o o o o o o o o o o		one tile (4x4 grid of bytes):
// 		o o o o o o o o o o o o X O O O o o o o o o o o o o o o o o o o		b b b b
// 		o o o o o o o o o o o o O O O O o o o o o o o o o o o o o o o o		b b b b
// 		o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o		b b b b
// 		o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o		b b b b
// 		o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o		
// 		o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o		each attribute table member maps directly on to a relative 4x4 tile grid
// 		o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o		in memory, an attribute table member is depicted as four dibits, for example:
// 		o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o		0b00 01 10 11
// 		o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o		each dibit contains palette information for four tiles (a block).
// 		o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o		in this example, this byte would be assigned to the block as follows:
// 		o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o		0b11	0b10
// 		o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o		O O		O O
// 		o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o		O O		O O
// 		o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o		0b01	0b00
// 		o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o		X X		O O
// 		o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o		X X		O O
//
//		
//	for example, byte 19 in the attribute table which is our targetted attribute table member is 0b00011011
//	 since our targetted tile in the nametable grid is the third tile in the grid (bottom left),
//	 the third lowest dibit (0b01) is the assigned background palette
std::vector<BYTE> NESDisplay::GetBgTilePalette(const std::vector<BYTE>& nametable, const size_t tileNoX, const size_t tileNoY)
{
	std::vector<BYTE> paletteColours{};

	/*if (tileNoX > 32 || tileNoY > 30)
	{
		std::cerr << "ERROR: Attempted to get invalid tile's palette!" << std::endl;
		return paletteColours; // function was misused
	}*/

	static const WORD ATTRIBUTE_TABLE_BEGIN = 0x3C0;

	// for example, tileNoX is 12 and tileNoY is 10, therefore offset is 19
	size_t attrTblOffset = (tileNoX / 4) + ((tileNoY / 4) * 8); // attribute table is an 8x8 grid
	BYTE paletteByte = nametable.at(ATTRIBUTE_TABLE_BEGIN + attrTblOffset);

	std::bitset<2> blockX = (tileNoX % 4) / 2;
	std::bitset<2> blockY = (tileNoY % 4) / 2;
	std::bitset<2> blockTilePos = ((blockY << 1) | blockX).to_ulong();

	// if position is 0, then grab the first lowest dibit
	// if position is 1, then grab the second lowest dibit, etc.
	size_t amountToShift = (2 * blockTilePos.to_ulong());
	std::bitset<2> paletteIndex = (paletteByte >> amountToShift) & 0b00000011;

	// first palette colour is always the same
	paletteColours.push_back(m_pPPU->GetPaletteRAM().at(0));

	size_t paletteStart = (paletteIndex.to_ullong() * 4);
	for (int i = 1; i < 4; ++i)
		paletteColours.push_back(m_pPPU->GetPaletteRAM().at(paletteStart + i));
	
	return paletteColours;
}

std::vector<BYTE> NESDisplay::GetSpriteTilePalette(const std::bitset<2>& paletteIndex)
{
	std::vector<BYTE> paletteColours{m_pPPU->GetPaletteRAM().at(0)}; // initialise with one member, which won't be used anyway
	static const WORD SPRITE_TABLE_OFFSET = 0x10;

	for (int i = 1; i < 4; ++i)
		paletteColours.push_back(m_pPPU->GetPaletteRAM().at(SPRITE_TABLE_OFFSET + (paletteIndex.to_ulong() * 4) + i));

	return paletteColours;
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

		std::vector<BYTE> tilePaletteIndexes = GetBgTilePalette( m_pPPU->GetNametableRAM()[bgBankAddr ? 1 : 0], screenPosX, screenPosY);
		DrawTile(*pCurTile, screenPosX * 8, screenPosY * 8, tilePaletteIndexes);
	}
}

void NESDisplay::DrawSprites()
{
	const WORD spriteBankAddr = dynamic_cast<PPUCTRL *>(m_pPPU->registers.ppuctrl.get())->GetSpritePTableAddr();

	for (int i = m_pPPU->GetOAM().size() - 4; i >= 0; i -= 4)
	{
		// https://www.nesdev.org/wiki/PPU_OAM
		// TODO: probably pack this into a struct, then replace "4" magic numbers with evaluation of sizeof(struct) / sizeof(BYTE)
		BYTE tileY = m_pPPU->GetOAM()[i];
		BYTE tileIndex = m_pPPU->GetOAM()[i + 1];
		OAMProperties tileProperties = (OAMProperties)m_pPPU->GetOAM()[i + 2];
		BYTE tileX = m_pPPU->GetOAM()[i + 3];

		std::bitset<2> paletteIndex = (tileProperties & (OAMProperties::PALETTE_HIGH | OAMProperties::PALETTE_LOW));
		bool shouldFlipVertical = (tileProperties & OAMProperties::FLIP_VERTICALLY);
		bool shouldFlipHorizontal = (tileProperties & OAMProperties::FLIP_HORIZONTALLY);

		const WORD selectedTileOffset = tileIndex * sizeof(Tile); // address of tile to be used
		const Tile *pCurTile = (const Tile *)(&(m_pPPU->GetCHR_ROM()->at(spriteBankAddr + selectedTileOffset)));

		std::vector<BYTE> paletteColours = GetSpriteTilePalette(paletteIndex);
		DrawTile(*pCurTile, tileX, tileY, paletteColours, true, shouldFlipVertical, shouldFlipHorizontal);
	}
}