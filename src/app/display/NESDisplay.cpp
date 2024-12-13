#include "NESDisplay.h"
// TODO: probably put x and y into a struct

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
	//DrawTiles(m_pPPU->GetCHR_ROM(), 0);
}

// TODO: add start x,y
void NESDisplay::DrawTiles(const std::vector<BYTE> *pCHR_ROM, const size_t bank)
{
	const std::vector<BYTE>& nametable = m_pPPU->GetNametableRAM()[0]; // temporarily use nametable 0 only
	const WORD tilesStartAddr = bank * 0x1000;

	size_t pixelPosY = 0;
	size_t pixelPosX = 0;
	std::vector<Tile> tiles{};
	for (int tileNo = 0; tileNo < 16*16; ++tileNo)
	{
		const WORD curTileOffset = tileNo * sizeof(Tile);
		const WORD curTilePos = tilesStartAddr + curTileOffset;

		if (tileNo && tileNo % 16 == 0)
		{
			pixelPosY += 8; // new line
			pixelPosX = 0;	 // reset x at end of line
		}

		// map 16 bytes at curTilePos into curTile
		const Tile *pCurTile = (const Tile *)(&(pCHR_ROM->at(curTilePos)));
		tiles.push_back(*pCurTile); // push_back makes a copy, so no problem dereferencing a temporary pointer

		const size_t tilePosX = tileNo % 32;
		const size_t tilePosY = tileNo / 32;

		DrawTile(*pCurTile, pixelPosX, pixelPosY, GetBgTilePalette(nametable, tilePosX, tilePosY));

		pixelPosX += 8;
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

// each pixel of a tile is coded using 16 bytes, because each pixel can be one of four colours.
// each pixel is therefore denoted by a nibble (4 bits) which determine its colour.
// each byte is a bitfield, where the first 8 bytes (left) contain 64 bits (the number of pixels in a tile)
// 	 that control the low bit of each nibble, whereas the next 8 bytes (right) contain 64 bits that control
//	 the high bit of each nibble.
// for a sprite, a nibble of value 0 is transparent.
// for example, to draw a smiley face using 16 bytes:

//		left (first 8 bytes)			right (last 8 bytes)
// byte  0: 0 0 0 0 0 0 0 0	    	byte  8: 0 0 0 0 0 0 0 0
// byte  1: 0 0 0 0 0 0 0 0	    	byte  9: 0 0 0 0 0 0 0 0
// byte  2: 0 1 0 0 0 0 1 0	    	byte 10: 0 0 0 0 0 0 0 0
// byte  3: 0 0 0 0 0 0 0 0	    	byte 11: 0 0 0 0 0 0 0 0
// byte  4: 0 0 0 0 0 0 0 0	    	byte 12: 0 0 0 0 0 0 0 0
// byte  5: 0 1 0 0 0 0 1 0	    	byte 13: 0 0 0 0 0 0 0 0
// byte  6: 0 0 1 1 1 1 0 0	    	byte 14: 0 0 1 1 1 1 0 0
// byte  7: 0 0 0 0 0 0 0 0	    	byte 15: 0 0 0 0 0 0 0 0


// 		result (combined left & right)
// nibble 0: 00 00 00 00 00 00 00 00
// nibble 1: 00 00 00 00 00 00 00 00
// nibble 2: 00 01 00 00 00 00 01 00
// nibble 3: 00 00 00 00 00 00 00 00
// nibble 4: 00 00 00 00 00 00 00 00
// nibble 5: 00 10 00 00 00 00 10 00
// nibble 6: 00 00 11 11 11 11 00 00
// nibble 7: 00 00 00 00 00 00 00 00

// therefore, the eyes are of one colour (tilePaletteIndex 1 (0b01)), and the mouth is of another
// colour (tilePaletteIndex 3 (0b11)). the corners of the mouth are of colour tilePaletteIndex 2 (0b10).
// where tilePosX = 0-32, tilePosY = 0-30
void NESDisplay::DrawTile(const Tile &tile, size_t tilePosX, size_t tilePosY, const std::vector<BYTE>& tilePaletteIndexes, bool isSprite, bool flipY, bool flipX)
{
	for (int curByte = 0; curByte < 8; ++curByte)
	{
		std::bitset<8> left = tile.left[curByte];
		std::bitset<8> right = tile.right[curByte];

		for (int curBit = 0; curBit < 8; ++curBit) // left to right
		{
			std::bitset<4> pixelNibble = (right[curBit] << 1) | left[curBit];
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
	static const WORD SPRITE_TABLE_OFFSET = 0x10; // sprite table is located at 0x3F10 - 0x3F20

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
		const std::vector<BYTE>& nametable = m_pPPU->GetNametableRAM()[0]; // temporarily use nametable 0 only

		if (tileNo >= nametable.size())
		{
			std::cerr << "Attempted to access tile outside of nametable!" << std::endl;
			break;
		}

		const WORD selectedTilePos = nametable.at(tileNo);				// position of tile to be used
		const WORD selectedTileOffset = selectedTilePos * sizeof(Tile); // address of tile to be used
		const Tile *pCurTile = (const Tile *)(&(m_pPPU->GetCHR_ROM()->at(bgBankAddr + selectedTileOffset)));

		const size_t tilePosX = tileNo % 32;
		const size_t tilePosY = tileNo / 32;

		std::vector<BYTE> tilePaletteIndexes = GetBgTilePalette( m_pPPU->GetNametableRAM()[0], tilePosX, tilePosY);
		DrawTile(*pCurTile, tilePosX * 8, tilePosY * 8, tilePaletteIndexes);
	}
}

void NESDisplay::DrawSprites()
{
	const WORD spriteBankAddr = dynamic_cast<PPUCTRL *>(m_pPPU->registers.ppuctrl.get())->GetSpritePTableAddr();
	// https://www.nesdev.org/wiki/PPU_sprite_priority
	// "Sprites with lower OAM indicies are drawn in front", therefore we loop through the OAM table from sprite 63 to sprite 0
	//   to render lower sprites on top of higher sprites
	for (int i = m_pPPU->GetOAM().size() - sizeof(SpriteData); i >= 0; i -= sizeof(SpriteData))
	{
		// https://www.nesdev.org/wiki/PPU_OAM
		const SpriteData *pCurSprite = (const SpriteData *)(&(m_pPPU->GetOAM().at(i)));

		std::bitset<2> paletteIndex = (pCurSprite->tileProperties & (OAMProperties::PALETTE_HIGH | OAMProperties::PALETTE_LOW));
		bool shouldFlipVertical = (pCurSprite->tileProperties & OAMProperties::FLIP_VERTICALLY);
		bool shouldFlipHorizontal = (pCurSprite->tileProperties & OAMProperties::FLIP_HORIZONTALLY);

		const WORD selectedTileOffset = pCurSprite->tileIndex * sizeof(Tile); // address of tile to be used
		const Tile *pCurTile = (const Tile *)(&(m_pPPU->GetCHR_ROM()->at(spriteBankAddr + selectedTileOffset)));

		std::vector<BYTE> paletteColours = GetSpriteTilePalette(paletteIndex);
		DrawTile(*pCurTile, pCurSprite->tileX, pCurSprite->tileY, paletteColours, true, shouldFlipVertical, shouldFlipHorizontal);
	}
}