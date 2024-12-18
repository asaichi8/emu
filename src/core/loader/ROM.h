#pragma once

#include <vector>
#include <stdexcept>
#include "Loader.h"
#include "../../include/typedefs.h"


// https://www.nesdev.org/wiki/INES#Nametable_Arrangement
// https://www.nesdev.org/wiki/Mirroring#Nametable_Mirroring
enum MirrorType
{
	Vertical = 0, // nametables are horizontal, resulting in vertical mirroring (64x30 tilemap)
	Horizontal,   // nametables are vertical, resulting in horizontal mirroring (32x60 tilemap)
	FourScreen
};


// https://www.nesdev.org/wiki/INES#Flags_6
enum Flags6 : BYTE
{
	NametableArrangement = 1 << 0,  
	HasBatteryPRG_RAM    = 1 << 1,   
	HasTrainer           = 1 << 2,               
	AltNametableLayout   = 1 << 3,
	MapperNoLow0         = 1 << 4,      
	MapperNoLow1         = 1 << 5,     
	MapperNoLow2         = 1 << 6,  
	MapperNoLow3         = 1 << 7
};


// https://www.nesdev.org/wiki/INES#Flags_7
enum Flags7 : BYTE
{
	VSUnisystem     = 1 << 0,  
	PlayChoice10    = 1 << 1,   
	NES2FormatFlag0 = 1 << 2,               
	NES2FormatFlag1 = 1 << 3,
	MapperNoHigh0   = 1 << 4,      
	MapperNoHigh1   = 1 << 5,     
	MapperNoHigh2   = 1 << 6,  
	MapperNoHigh3   = 1 << 7
};


class ROM
{
	#pragma pack(push, 1) // ensure no padding
	struct iNES_Header
	{
		BYTE signature[4];
		BYTE PRG_ROM_banks;
		BYTE CHR_ROM_banks;
		Flags6 flags6;
		Flags7 flags7;
		BYTE PRG_RAM_size;
		BYTE TV_System;
		BYTE reserved[6];
	};
	#pragma pack(pop)
	
	const BYTE NES_SIG[4] = {'N', 'E', 'S', 0x1A}; // "NES^Z"
	const size_t TRAINER_SIZE = 512;
	const size_t PRG_ROM_BANK_SIZE = 16 * KB;
	const size_t CHR_ROM_BANK_SIZE = 8 * KB;

	std::unique_ptr<std::vector<BYTE>> m_pRawFile{};
	iNES_Header m_inesHeader{};
	size_t m_nPRG_ROM_size{};
	size_t m_nCHR_ROM_size{};

	bool CheckHeaderFits(const std::vector<BYTE> rawFile);
	bool CheckHeaderIsINES(const iNES_Header* header);
	bool CheckFileSize(const std::vector<BYTE>& rawFile, const iNES_Header* header);
	BYTE GetINESVersion(const iNES_Header* header);
	BYTE GetMapperType(const iNES_Header* header);
	MirrorType GetMirrorType(const iNES_Header* header);

public:
	ROM();
	
	std::vector<BYTE> PRG_ROM{};
	std::vector<BYTE> CHR_ROM{};
	BYTE mapperType{};
	MirrorType mirrorType{};
	
	std::string CheckROM(const std::string& filePath, bool shouldUse = false);
	std::string LoadROM(const std::string& filePath);
	const std::vector<BYTE>* GetRawFile() const { return m_pRawFile.get(); }
};
