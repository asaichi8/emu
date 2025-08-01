#include "ROM.h"


ROM::ROM()
{

}




// https://www.nesdev.org/wiki/INES

/// @brief Loads the file and checks if it is a valid .nes file.
/// @param filePath 
/// @return The error message. Succeeded if nothing was returned.
std::string ROM::CheckROM(std::unique_ptr<std::vector<BYTE>> pRomRaw, bool shouldUse, const Loader::GameInfo& info)
{
	// if shouldUse, we move the pointer to the member variable
	if (shouldUse)
	{
		m_pRawFile = std::move(pRomRaw);
		m_gameInfo = info;
	}

	// avoid creating two vectors of bytes by copying the pointer - use one pointer to refer to either the
	//   moved raw file pointer, or the copied raw file pointer
	std::vector<BYTE>* pFile = shouldUse ? m_pRawFile.get() : pRomRaw.get();


	// check we actually loaded something
	if (pFile->empty())
		return "Cannot load an empty file!";

	// check size
	if (!CheckHeaderFits(*pFile))
		return "File not large enough to read header!";

	// map first 16 bytes to iNES_Header
	iNES_Header* header = (iNES_Header*)(pFile->data());
	if (shouldUse)
		m_inesHeader = *header; // create a copy of the header to store as a member variable
	
	// check that the file is an iNES file and file actually has space for data requested by header
	if (!CheckINES(header))
	{
		if (!CheckINES((iNES_Header*)(info.szInesHeader))) // no backup header
			return "Not an NES file!";

		// we have a backup header meaning this is probably a headerless NES file, so let's try to insert the header ourselves
		LOG_DEBUG("headerless file, inserting header...");
		pFile->insert(pFile->begin(), &info.szInesHeader[0], &info.szInesHeader[16]);
		header = (iNES_Header*)(pFile->data()); // now get the header again

		// if shouldUse, we want to modify our stored raw file and header too
		if (shouldUse)
			m_inesHeader = *header;
	}

	if (!CheckFileSize(*pFile, header))
		return "File not large enough to map data requested by header!";
	// if (GetINESVersion(header) > 0) 
	// 	return "Only iNES 1.0 is supported at this time!";

	return {};
}

/// @brief Maps ROM into memory.
void ROM::MapROM()
{
	if (!m_pRawFile)
		return;

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
}

void ROM::UpdateHash()
{
	if (!m_pRawFile)
		return;

	m_MD5pair = DatabaseHandler::GetMD5(m_pRawFile.get());
}

/// @brief Mirrors game genie codes to the PRG_ROM's size, so they line up when we access them later
void ROM::MirrorGameGenieCodes()
{
	if (PRG_ROM.empty())
		return;
		
	for (auto& code : m_gameInfo.gameGenieCodes)
	{
		for (auto& decodedCode : code.decoded)
		{
			decodedCode.addr %= PRG_ROM.size();
		}
	}
}


bool ROM::CheckHeaderFits(const std::vector<BYTE> rawFile)
{
	return rawFile.size() > sizeof(iNES_Header);
}

bool ROM::CheckINES(const iNES_Header* header)
{
	return (memcmp(header->signature, NES_SIG, 4) == 0);
}
bool ROM::CheckINES(const std::vector<BYTE>* romRaw)
{
	BYTE sig[4];

	// fill sig with first 4 bytes of ROM
	for (int i = 0; i < 4; ++i)
	{
		sig[i] = romRaw->at(i);
	}

	return (memcmp(sig, NES_SIG, sizeof(NES_SIG)) == 0);
}

/// @brief Ensures that a given file abides by the size promised in the header.
/// @param rawFile Raw file to be checked against.
/// @param header The iNES header for the file.
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

// TODO: https://www.nesdev.org/wiki/INES#Variant_comparison
// see "recommended detection procedure"
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