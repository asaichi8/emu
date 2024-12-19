#pragma once

#include <cstring>
#include <vector>
#include <sstream>
#include <fstream>
#include <cstdint>
#include <iostream>
#include <filesystem>
#include <openssl/evp.h>
#include <nlohmann/json.hpp>
#include "../../include/typedefs.h"
#include "../cheats/GameGenie.h"


class Loader
{
	static std::string HashToHexStr(const BYTE hash[], size_t len);
	static std::string CalcMD5(const std::vector<BYTE>& file);
	static bool IsNESFile(const std::vector<BYTE>* romRaw);

public:
	Loader();

	struct GameInfo
	{
		std::string name{};
		// probably don't need md5 hash stuff
		//std::string md5{}; 
		//std::string md5headerless{};
		std::vector<GameGenie::GameGenieCode> gameGenieCodes{};
		BYTE szInesHeader[16]{};

		void inline SetHeaderFromHexStr(const std::string& hexStr)
		{
			if (hexStr.size() != 32)
				throw std::invalid_argument("Invalid hex string size!");
			
			for (int i = 0; i < 16; ++i)
			{
				std::string strCurByte = hexStr.substr(i * 2, 2);
				if (!std::isxdigit(strCurByte.at(0)) || !std::isxdigit(strCurByte.at(1)))
					throw std::invalid_argument("Hex string does not contain hex characters!");
				
				szInesHeader[i] = (BYTE)(std::stoi(strCurByte, nullptr, 16));
			}
		}

		// if the header doesn't begin with 'N', the header was loaded - header always starts with "NES"
		const bool inline IsHeaderNull() const { return szInesHeader[0] != 'N'; }
	};

	static std::vector<BYTE> HexStrToBytes(const std::string& hexString);
	static std::vector<BYTE> LoadFile(const std::string& filePath);
	static std::string GetFullFilePath(const std::string& relativePath);
	static Loader::GameInfo FindROM(const std::vector<BYTE>* romRaw, const std::string& dbPath);
};
