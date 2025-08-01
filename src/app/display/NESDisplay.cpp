#include "NESDisplay.h"
// TODO: clean this up. split DrawPixel into two functions for sprite and for nametable, maybe make sprite/nametable class

NESDisplay::NESDisplay(const PPU* pPPU, Palette* pPalette) : m_pPPU(pPPU), m_pPalette(pPalette)
{

}

NESDisplay::~NESDisplay()
{

}


void NESDisplay::DrawScreen()
{    
    WORD nametableAddr = dynamic_cast<PPUCTRL *>(m_pPPU->registers.ppuctrl.get())->GetNametableAddr();
    bool useFirstNametable = !m_pPPU->GetNametableIndex(nametableAddr);
	
    Point scroll = {
        dynamic_cast<PPUSCROLL *>(m_pPPU->registers.ppuscroll.get())->GetX(),
        dynamic_cast<PPUSCROLL *>(m_pPPU->registers.ppuscroll.get())->GetY()
    };

	DrawNametable(m_pPPU->GetNametableRAM()[!useFirstNametable], false, scroll, {DISPLAY_WIDTH, DISPLAY_HEIGHT}, scroll * -1); 
	// TODO: support four scroll
	if      (scroll.x) DrawNametable(m_pPPU->GetNametableRAM()[useFirstNametable], false, {0, 0}, {scroll.x, DISPLAY_HEIGHT}, {DISPLAY_WIDTH - scroll.x, 0});
	else if (scroll.y) DrawNametable(m_pPPU->GetNametableRAM()[useFirstNametable], false, {0, 0}, {DISPLAY_WIDTH, scroll.y}, {0, DISPLAY_HEIGHT - scroll.y});

	// FOR NAMETABLE DISPLAY TESTING, remove when not used
	// tile display
	DrawTiles(m_pPPU->GetCHR_ROM(), 0, 1, {0, 64}, 16);
	DrawTiles(m_pPPU->GetCHR_ROM(), 1, 1, {128, 64}, 16);
	// nametable display
	DrawNametable(m_pPPU->GetNametableRAM()[!useFirstNametable], 2);
	DrawNametable(m_pPPU->GetNametableRAM()[useFirstNametable], 3);
	//vertical lines
	for (int i = 0; i < 240; ++i)
	{
		if (scroll.x > 0) SetPixel({255, 0, 0}, {scroll.x - 1, i}, {0, 0, 0}, false, 2);
		if (scroll.x < 255) SetPixel({255, 0, 0}, {scroll.x, i}, {0, 0, 0}, false, 3);
	}
	//horizontal lines
	for (int i = 0; i < 256; ++i)
	{
		if (scroll.x <= i) 
		{
			SetPixel({255, 0, 0}, {i, 239}, {0, 0, 0}, false, 2);
			SetPixel({255, 0, 0}, {i, 0}, {0, 0, 0}, false, 2);
		}

		if (scroll.x > i)
		{
			SetPixel({255, 0, 0}, {i, 239}, {0, 0, 0}, false, 3);
			SetPixel({255, 0, 0}, {i, 0}, {0, 0, 0}, false, 3);
		}
	}

    DrawSprites();
    // DrawTiles(m_pPPU->GetCHR_ROM(), 0);
}

void NESDisplay::DrawTiles(const std::vector<BYTE> *pCHR_ROM, const size_t bank, size_t bufNum, const Point& startPos, size_t lineLength)
{
	if (bank > 2)
	{
		LOG_ERROR("Invalid bank!");
		return;
	}

	const std::vector<BYTE>& nametable = m_pPPU->GetNametableRAM()[0]; // temporarily use nametable 0 only
	const WORD tilesStartAddr = bank * 0x1000;

	Point pixelPos = startPos;
	std::vector<Tile> tiles{};
	for (int tileNo = 0; tileNo < 16*16; ++tileNo)
	{
		const size_t curTileOffset = tileNo * sizeof(Tile);
		const size_t curTilePos = tilesStartAddr + curTileOffset;

		if (tileNo && tileNo % lineLength == 0)
		{
			pixelPos.y += 8; // new line
			pixelPos.x = startPos.x;	 // reset x at end of line
		}

		// map 16 bytes at curTilePos into curTile
		const Tile *pCurTile = (const Tile *)(&(pCHR_ROM->at(curTilePos)));
		tiles.push_back(*pCurTile); // push_back makes a copy, so no problem dereferencing a temporary pointer

		const Point tilePos = {tileNo % 32, tileNo / 32};

		DrawTile(*pCurTile, pixelPos, GetBgTilePalette(nametable, tilePos), bufNum);

		pixelPos.x += 8;
	}
}


bool NESDisplay::SetPixel(const RGB colour, const Point& pixelPos, RGB transparentColour, bool behindBg, size_t bufNum)
{
	const size_t pixelStartPos = (pixelPos.x * sizeof(RGB)) + (pixelPos.y * sizeof(RGB) * DISPLAY_WIDTH);
	if (pixelStartPos + sizeof(RGB) > SCREEN_BUFFER_SIZE)
		return false; // not fatal

	if (bufNum >= 4) bufNum = 0; // safeguard in case we access value out of bufNum size
	BYTE* bufPtr = m_szScreenBuffer[bufNum];

	if (behindBg && (
			(*(RGB*)(&bufPtr[pixelStartPos])).r != transparentColour.r ||
			(*(RGB*)(&bufPtr[pixelStartPos])).g != transparentColour.g ||
			(*(RGB*)(&bufPtr[pixelStartPos])).b != transparentColour.b
		))
		return false;

	// directly map the RGB colour struct into memory
	*(RGB*)(&bufPtr[pixelStartPos]) = colour;

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
void NESDisplay::DrawTile(const Tile &tile, const Point& tilePos, const std::vector<BYTE>& tilePaletteIndexes, size_t bufNum,
						  const Point& start, const Point& end, const Point& shift,
						  bool isSprite, bool flipY, bool flipX, bool behindBg)
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

			Point pixelOffset{};
			pixelOffset.x = flipX ? curBit : 7 - curBit; // tiles are flipped on the x axis by default
			pixelOffset.y = flipY ? 7 - curByte : curByte;
			
			Point pixelPos = tilePos + pixelOffset;
			if (pixelPos.x < start.x || pixelPos.y < start.y || pixelPos.x >= end.x || pixelPos.y >= end.y)
				continue;

			if ((pixelPos.x + shift.x) < 0) continue;
			if ((pixelPos.y + shift.y) < 0) continue;
			if ((pixelPos.x + shift.x) > DISPLAY_WIDTH) continue;
			if ((pixelPos.y + shift.y) > DISPLAY_HEIGHT) continue;
			
			SetPixel(rgb, pixelPos + shift, m_pPalette->GetPalette().at(tilePaletteIndexes.at(0)), behindBg, bufNum);
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
std::vector<BYTE> NESDisplay::GetBgTilePalette(const std::vector<BYTE>& nametable, const Point& tileNo)
{
	std::vector<BYTE> paletteColours{};

	/*if (tileNoX > 32 || tileNoY > 30)
	{
		LOG_WARN("Attempted to get invalid tile's palette!");
		return paletteColours; // function was misused
	}*/

	static const WORD ATTRIBUTE_TABLE_BEGIN = 0x3C0;

	// for example, tileNoX is 12 and tileNoY is 10, therefore offset is 19
	size_t attrTblOffset = (tileNo.x / 4) + ((tileNo.y / 4) * 8); // attribute table is an 8x8 grid
	BYTE paletteByte = nametable.at(ATTRIBUTE_TABLE_BEGIN + attrTblOffset);

	std::bitset<2> blockX = (tileNo.x % 4) / 2;
	std::bitset<2> blockY = (tileNo.y % 4) / 2;
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

void NESDisplay::DrawNametable(const std::vector<BYTE>& nametable, size_t bufNum, const Point& start, const Point& end, const Point& shift)
{
	// bgBankAddr either 0 or 0x1000, use it to determine which bank we access
	const WORD bgBankAddr = dynamic_cast<PPUCTRL *>(m_pPPU->registers.ppuctrl.get())->GetBackgroundPTableAddr();

	// LOG_DEBUG("bgbankaddr: 0x" << std::hex << std::setw(4) << std::setfill('0') << bgBankAddr << " nametableaddr: 0x" <<
	//  dynamic_cast<PPUCTRL *>(m_pPPU->registers.ppuctrl.get())->GetNametableAddr());

	std::vector<Tile> tiles{};
	// NES screen is 32 tiles long, 30 tiles high (32 * 8 = 256, 30 * 8 = 240)
	for (int tileNo = 0; tileNo < 32 * 30; ++tileNo)
	{
		const Point tilePos = {tileNo % 32, tileNo / 32};

		// prevent attempting to draw a tile out of bounds prematurely to avoid lag
		if ((tilePos.x * 8) > end.x || ((tilePos.x + 1) * 8 < start.x) || (tilePos.y * 8) > end.y || (tilePos.y + 1) * 8 < start.y)
			continue;

		if (tileNo >= nametable.size())
		{
			LOG_WARN("Attempted to access tile outside of nametable!");
			break;
		}

		const size_t selectedTilePos = nametable.at(tileNo);				// position of tile to be used
		const size_t selectedTileOffset = selectedTilePos * sizeof(Tile); // address of tile to be used
		const size_t selectedTileIndex = bgBankAddr + selectedTileOffset; // index in CHR_ROM of tile to be used

		if (m_pPPU->GetCHR_ROM()->size() + sizeof(Tile) < selectedTileIndex)
		{
			LOG_WARN("Attempted to access nametable tile outside of CHR_ROM!");
			continue;
		}

		const Tile *pCurTile = (const Tile *)(&(m_pPPU->GetCHR_ROM()->at(selectedTileIndex)));

		std::vector<BYTE> tilePaletteIndexes = GetBgTilePalette(nametable, tilePos);

		DrawTile(*pCurTile, tilePos * 8, tilePaletteIndexes, bufNum, start, end, shift);
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

		const size_t selectedTileOffset = pCurSprite->tileIndex * sizeof(Tile); // address of tile to be used
		const size_t selectedTileIndex = spriteBankAddr + selectedTileOffset;   // index in CHR_ROM of tile to be used

		if (selectedTileIndex + sizeof(Tile) > m_pPPU->GetCHR_ROM()->size())
		{
			LOG_WARN("Attempted to access sprite tile outside of CHR_ROM!");
			continue;
		}

		const Tile *pCurTile = (const Tile *)(&(m_pPPU->GetCHR_ROM()->at(selectedTileIndex)));
		// add 1 to tileY, because sprite data is delayed by one scanline in the NES: https://www.youtube.com/watch?v=8M2V5Pv6_1A&t=294s
		Point tilePos = {pCurSprite->tileX, pCurSprite->tileY + 1};
		std::vector<BYTE> paletteColours = GetSpriteTilePalette(paletteIndex);

		DrawTile(*pCurTile, tilePos, paletteColours, false,
				 {0, 0}, {DISPLAY_WIDTH, DISPLAY_HEIGHT}, {0, 0},
				 true, shouldFlipVertical, shouldFlipHorizontal, pCurSprite->tileProperties & OAMProperties::PRIORITY);
	}
}