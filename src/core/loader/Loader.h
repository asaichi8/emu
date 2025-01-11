#pragma once

#include <cstring>
#include <vector>
#include <sstream>
#include <fstream>
#include <cstdint>
#include <filesystem>
#include <unordered_map>
#include <openssl/evp.h>
#include <nlohmann/json.hpp>
#include "../../include/pch.h"
#include "../cheats/GameGenie.h"


class Loader
{
	static const size_t NES_HEADER_SIZE = 16;

public:
	struct GameInfo
	{
		std::string name{};
		std::vector<GameGenie::GameGenieCode> gameGenieCodes{};
		BYTE szInesHeader[16]{};
		// std::unordered_map<WORD, GameGenie::GameGenieCode*> activeCodeMap{};

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

		// To execute game genie codes, we need to access gameGenieCodes every time we read PRG ROM. Since we are reading PRG ROM a lot, it makes
		// sense to attempt to optimise this access, especially since the code map could be very large. Therefore, every time we update whether a code
		// is active or not, we call BuildCodeMap to build a hash map of active codes by address. Then, we can access a code by its address in a
		// very quick fashion, as opposed to looping through the entire gameGenieCodes vector and checking if each one is active or not.
		// void inline BuildCodeMap()
		// {
		// 	activeCodeMap.clear();

		// 	for (auto& code : gameGenieCodes)
		// 	{
		// 		if (!code.isActive)
		// 			continue;

		// 		activeCodeMap[code.decoded.addr] = (&code);
		// 	}
		// }
	};

	static std::vector<BYTE> HexStrToBytes(const std::string& hexString);
	static std::vector<BYTE> LoadFile(const std::string& filePath);
	static std::string GetFullFilePath(const std::string& relativePath);
};
