#include "Loader.h"


Loader::Loader()
{

}


std::vector<BYTE> Loader::HexStrToBytes(const std::string& hexString) 
{
	std::vector<BYTE> bytes;
	std::istringstream stream(hexString);
	std::string byteStr;

	while (stream >> byteStr)
	{
		WORD w;
		std::stringstream hexStream(byteStr);
		hexStream >> std::hex >> w;
		bytes.push_back((BYTE)w);
	}

	return bytes;
}

std::vector<BYTE> Loader::LoadFile(const std::string& filePath) 
{
	std::ifstream file(filePath, std::ios::binary | std::ios::ate);
	if (!file) 
	{
		std::cerr << "Failed to load file (does it exist?)" << std::endl;
		return {};
	}

	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);

	std::vector<BYTE> buffer(size);
	if (!file.read((char*)(buffer.data()), size)) 
	{
		std::cerr << "Failed to read from file!" << std::endl;
		return {};
	}

	return buffer;
}

std::string Loader::GetFullFilePath(const std::string& relativePath)
{
	std::filesystem::path executablePath = std::filesystem::current_path();
	return ((std::filesystem::path)(executablePath / relativePath)).string();
}

std::string Loader::CalcMD5(const std::vector<BYTE>& file)
{
	BYTE hash[EVP_MAX_MD_SIZE];
	size_t hashLength{};

	if (!EVP_Q_digest(nullptr, "MD5", nullptr, file.data(), file.size(), hash, &hashLength))
	{
		std::cerr << "Failed to calculate MD5 hash!" << std::endl;
		return {};
	}

	return HashToHexStr(hash, hashLength);
}


std::string Loader::HashToHexStr(const BYTE hash[], size_t len)
{
	std::stringstream ss;

	for (int i = 0; i < len; ++i)
	{
		ss << std::hex << std::setw(2) << std::setfill('0') << (int)(hash[i]);
	}

	return ss.str();
}

Loader::GameInfo Loader::FindROM(const std::vector<BYTE>* romRaw, const std::string& dbPath)
{
	GameInfo info{};

	if (romRaw->empty() || dbPath.empty())
	{
		std::cerr << "Invalid usage" << std::endl;
		return {};
	}

    std::string md5 = Loader::CalcMD5(*romRaw);
	std::cout << "md5: " << md5 << std::endl;
	if (md5.empty())
		return {};


	std::ifstream db(dbPath);
	if (!db.is_open())
	{
		std::cerr << "Could not open JSON database!" << std::endl;
		return {};
	}

	using json = nlohmann::json;
	json j;
	db >> j;

	if (!j.contains("roms") || !j["roms"].is_array())
	{
		std::cerr << "Invalid JSON format!" << std::endl;
		return {};
	}

	for (const auto& rom : j["roms"])
	{
		if (!rom.contains("md5") || rom["md5"] != md5)
			continue;
			
		if (rom.contains("name"))
			info.name = rom["name"].get<std::string>();
		else
			info.name = "Unknown";
		
		if (rom.contains("game_genie_codes") && rom["game_genie_codes"].is_array())
		{
			for (const auto& codeStruct : rom["game_genie_codes"])
			{
				if (!codeStruct.contains("code") || !codeStruct.contains("description"))
					continue;
				
				std::string strCode = codeStruct["code"];
				std::string strDescription = codeStruct["description"];

				bool isActive = false;
				if (codeStruct.contains("is_active"))
					isActive = codeStruct["is_active"].get<bool>();

				info.gameGenieCodes.emplace_back(strCode, strDescription, isActive);
			}
		}

		return info;
	}
	
	std::cerr << "couldn't find hash" << std::endl;
    return {};
}