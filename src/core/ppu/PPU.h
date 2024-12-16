#pragma once

#include "../memory/Bus.h"
#include "../loader/ROM.h"
#include "registers/IPPURegister.h"

// https://www.nesdev.org/wiki/PPU_memory_map
#define PATTERN_TABLE_SIZE 0x1000
#define PATTERN_TABLES_TOTAL_SIZE (PATTERN_TABLE_SIZE * 2) // 0x2000
#define PATTERN_TABLES_END PATTERN_TABLES_TOTAL_SIZE

#define NAMETABLES_BEGIN 0x2000
#define NAMETABLE_SIZE 0x0400
#define NAMETABLES_TOTAL_SIZE (NAMETABLE_SIZE * 4) // 0x1000 - nametable is mirrored
#define NAMETABLES_END NAMETABLES_BEGIN + NAMETABLES_TOTAL_SIZE
#define NAMETABLES_MIRRORED_END 0x3F00

#define PALETTE_RAM_BEGIN NAMETABLES_MIRRORED_END // 0x3F00
#define PALETTE_SIZE 0x04 // https://www.nesdev.org/wiki/PPU_palettes#Palette_RAM
#define PALETTE_RAM_TOTAL_SIZE (PALETTE_SIZE * 8)
#define PALETTE_RAM_END PALETTE_RAM_BEGIN + PALETTE_RAM_TOTAL_SIZE // 0x3F20
#define PALETTE_RAM_MIRRORED_END 0x4000

#define PPU_ADDRESS_SPACE_END 0x4000


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
	
	// https://www.nesdev.org/wiki/PPU_OAM#Byte_2 : set unused bytes to 0
	SpriteData() { tileProperties = (OAMProperties)(tileProperties & (BYTE)0b11100011); }
};
#pragma pack(pop)

class PPU
{
	std::vector<BYTE> m_NametableRAM[2];
	// dbg purposes only
	//std::vector<bool> TEST_NameTableRAMIsRealZero[2]; // set to true if accompanying byte in m_NametableRAM was really set to zero or just assigned that
	std::vector<BYTE> m_OAM{};
	std::vector<BYTE> m_PaletteRAM{};

	std::vector<BYTE>* m_pCHR_ROM;
	MirrorType* m_pMirrorType;

	IPPURegister::InternalRegisters internal_registers;

	DWORD m_nPPUCycles{};
	int m_nScanlines{};
	bool m_bShouldNMIInterrupt = false;

	struct Registers {
		std::unique_ptr<IPPURegister> ppuctrl 	;
		std::unique_ptr<IPPURegister> ppumask 	;
		std::unique_ptr<IPPURegister> ppustatus ;
		std::unique_ptr<IPPURegister> oamaddr 	;
		std::unique_ptr<IPPURegister> oamdata 	;
		std::unique_ptr<IPPURegister> ppuscroll ;
		std::unique_ptr<IPPURegister> ppuaddr 	;
		std::unique_ptr<IPPURegister> ppudata 	;
		std::unique_ptr<IPPURegister> oamdma 	;

		// TODO: should every PPU registers have access to its internal registers?
		Registers(IPPURegister::InternalRegisters* i)
		{
			ppuctrl 	= std::make_unique<PPUCTRL>	 (nullptr);
			ppumask 	= std::make_unique<PPUMASK>	 (nullptr);
			ppustatus 	= std::make_unique<PPUSTATUS>(i);
			oamaddr 	= std::make_unique<OAMADDR>	 (nullptr);
			oamdata 	= std::make_unique<OAMDATA>	 (nullptr);
			ppuscroll 	= std::make_unique<PPUSCROLL>(i);
			ppuaddr 	= std::make_unique<PPUADDR>	 (i);
			ppudata 	= std::make_unique<PPUDATA>	 (i);
			oamdma 		= std::make_unique<OAMDMA>	 (nullptr);
		}
	};	 

	WORD GetMirroredPPUADDRAddress(bool shouldIncrement = false);
	WORD MirrorPaletteRAMAddress(WORD addr);
	
public:
	PPU(std::vector<BYTE>* _pCHR_ROM, MirrorType* _pMirrorType);

	Registers registers; // TODO: this should probably be private, create (mostly) 1 liner functions instead

	bool Clock(DWORD nCycles);
	bool IsNMIInterruptQueued();
	BYTE ReadPPUByte();
	void WritePPUByte(BYTE val);
 
	DWORD GetCycleCount() const { return m_nPPUCycles; }
	int GetScanlineCount() const { return m_nScanlines; }
	bool GetInterruptStatus() const { return m_bShouldNMIInterrupt; }
	const std::vector<BYTE>* GetNametableRAM() const { return m_NametableRAM; }
	size_t GetNametableIndex(WORD addr);
	const std::vector<BYTE>* GetCHR_ROM() const { return m_pCHR_ROM; }
	const std::vector<BYTE>& GetPaletteRAM() const { return m_PaletteRAM; }
	const std::vector<BYTE>& GetOAM() const { return m_OAM; }
	const MirrorType* GetMirrorType() const { return m_pMirrorType; }
	
	void WriteOAM(BYTE addr, BYTE val) { m_OAM.at(addr) = val; }
	void PPUCtrlWriteW(BYTE val);
};
