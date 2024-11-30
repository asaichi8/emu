#include "ROM.h"


ROM::ROM()
{

}


// https://www.nesdev.org/wiki/INES

/// @brief 
/// @param filePath 
/// @return True if success, false if otherwise 
bool ROM::LoadROM(const std::string& filePath)
{
	// load file
	auto rawFile = LoadFile(filePath);

	// check size
	if (!CheckHeaderFits(rawFile))
		return false;

	// map first 16 bytes to iNES_Header
	const iNES_Header* header = (const iNES_Header*)(rawFile.data());
	
	// check that the file is an iNES file and file actually has space for data requested by header
	if (!CheckHeaderIsINES(header) || !CheckFileSize(rawFile, header))
		return false;

	if (GetINESVersion(header) > 0) 
	{
		std::cerr << "Only iNES 1.0 is supported at this time!" << std::endl;
		return false;
	}
	
	mapperType = GetMapperType(header);
	mirrorType = GetMirrorType(header);

	size_t PRG_ROM_start = sizeof(iNES_Header) + ((header->flags6 & Flags6::HasTrainer) ? TRAINER_SIZE : 0);
	size_t CHR_ROM_start = PRG_ROM_start + (header->PRG_ROM_banks * PRG_ROM_BANK_SIZE);

	auto PRG_begin = rawFile.begin() + PRG_ROM_start;
	auto PRG_end   = rawFile.begin() + PRG_ROM_start + (header->PRG_ROM_banks * PRG_ROM_BANK_SIZE);
	auto CHR_begin = rawFile.begin() + CHR_ROM_start;
	auto CHR_end   = rawFile.begin() + CHR_ROM_start + (header->CHR_ROM_banks * CHR_ROM_BANK_SIZE);

	PRG_ROM.assign(PRG_begin, PRG_end);
	CHR_ROM.assign(CHR_begin, CHR_end); 

	PRG_ROM = std::vector<BYTE>(PRG_begin, PRG_end);
	CHR_ROM = std::vector<BYTE>(CHR_begin, CHR_end);

	return true;
}

bool ROM::CheckHeaderFits(const std::vector<BYTE> rawFile)
{
	if (rawFile.size() <= sizeof(iNES_Header))
	{
		std::cerr << "File not large enough to read header!" << std::endl;
		return false;
	}
	
	return true;
}

bool ROM::CheckHeaderIsINES(const iNES_Header* header)
{
	if (memcmp(header->signature, NES_SIG, 4) != 0)
	{
		std::cerr << "Not an NES file!" << std::endl;
		return false;
	}
	
	return true;
}

/// @brief 
/// @param rawFile 
/// @param header 
/// @return True if valid, false if otherwise
bool ROM::CheckFileSize(const std::vector<BYTE>& rawFile, const iNES_Header* header) 
{
	size_t totalExpectedSize = sizeof(iNES_Header);

	if (header->flags6 & Flags6::HasTrainer) 
		totalExpectedSize += TRAINER_SIZE;

	totalExpectedSize += header->PRG_ROM_banks * PRG_ROM_BANK_SIZE;
	totalExpectedSize += header->CHR_ROM_banks * CHR_ROM_BANK_SIZE;

	if (!(rawFile.size() >= totalExpectedSize))
	{
		std::cerr << "File not large enough to map data requested by header!" << std::endl;
		return false;
	}

	return true;
}

BYTE ROM::GetINESVersion(const iNES_Header* header)
{
	return  (header->flags7 & Flags7::NES2FormatFlag1) || 
			(header->flags7 & Flags7::NES2FormatFlag0);
}

BYTE ROM::GetMapperType(const iNES_Header* header)
{
	return ((header->flags7 & 0xF0) >> 4) | (header->flags6 >> 4);
}

MirrorType ROM::GetMirrorType(const iNES_Header* header)
{
	if (header->flags6 & Flags6::AltNametableLayout)
		return MirrorType::FourScreen;
	else if (header->flags6 & Flags6::NametableArrangement)
		return MirrorType::Vertical;
	else
		return MirrorType::Horizontal;
}