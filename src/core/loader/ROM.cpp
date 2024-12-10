#include "ROM.h"


ROM::ROM()
{

}


// https://www.nesdev.org/wiki/INES

/// @brief Checks if the file loaded is a valid .nes file.
/// @param filePath 
/// @return The error message. Succeeded if nothing was returned.
std::string ROM::CheckROM(const std::string& filePath, bool shouldUse)
{
	// avoid creating two vectors of bytes by using a pointer
	std::unique_ptr<std::vector<BYTE>> pTempFile = std::make_unique<std::vector<BYTE>>(Loader::LoadFile(filePath));
	std::vector<BYTE>* pFile = pTempFile.get();

	if (shouldUse)
		m_pRawFile = std::move(pTempFile);

	// check we actually loaded something
	if (pFile->empty())
		return "Cannot load an empty file!";

	// check size
	if (!CheckHeaderFits(*pFile))
		return "File not large enough to read header!";

	// map first 16 bytes to iNES_Header
	const iNES_Header* header = (const iNES_Header*)(pFile->data());
	if (shouldUse)
		m_inesHeader = *header; // create a copy of the header to store as a member variable
	
	// check that the file is an iNES file and file actually has space for data requested by header
	if (!CheckHeaderIsINES(header))
		return "Not an NES file!";
	if (!CheckFileSize(*pFile, header))
		return "File not large enough to map data requested by header!";
	if (GetINESVersion(header) > 0) 
		return "Only iNES 1.0 is supported at this time!";

	return {};
}

/// @brief Maps ROM into memory.
/// @param filePath 
/// @return The error message. Succeeded if nothing was returned.
std::string ROM::LoadROM(const std::string& filePath)
{
	// CheckROM fills out m_rawFile & m_inesHeader
	std::string errMsg = CheckROM(filePath, true);
	if (!errMsg.empty())
		return errMsg;

	mapperType = GetMapperType(&m_inesHeader);
	mirrorType = GetMirrorType(&m_inesHeader);

	size_t PRG_ROM_start = sizeof(iNES_Header) + ((m_inesHeader.flags6 & Flags6::HasTrainer) ? TRAINER_SIZE : 0);
	size_t CHR_ROM_start = PRG_ROM_start + (m_inesHeader.PRG_ROM_banks * PRG_ROM_BANK_SIZE);

	auto PRG_begin = m_pRawFile->begin() + PRG_ROM_start;
	auto PRG_end   = m_pRawFile->begin() + PRG_ROM_start + (m_inesHeader.PRG_ROM_banks * PRG_ROM_BANK_SIZE);
	auto CHR_begin = m_pRawFile->begin() + CHR_ROM_start;
	auto CHR_end   = m_pRawFile->begin() + CHR_ROM_start + (m_inesHeader.CHR_ROM_banks * CHR_ROM_BANK_SIZE);

	PRG_ROM.assign(PRG_begin, PRG_end);
	CHR_ROM.assign(CHR_begin, CHR_end); 

	PRG_ROM = std::vector<BYTE>(PRG_begin, PRG_end);
	CHR_ROM = std::vector<BYTE>(CHR_begin, CHR_end);

	return {};
}


bool ROM::CheckHeaderFits(const std::vector<BYTE> rawFile)
{
	return rawFile.size() > sizeof(iNES_Header);
}

bool ROM::CheckHeaderIsINES(const iNES_Header* header)
{
	return (memcmp(header->signature, NES_SIG, 4) == 0);
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

	return (rawFile.size() >= totalExpectedSize);
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