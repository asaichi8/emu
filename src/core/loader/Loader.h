#pragma once

#include <cstring>
#include <vector>
#include <sstream>
#include <fstream>
#include <cstdint>
#include <iostream>
#include <filesystem>
#include <unordered_map>
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
		std::vector<GameGenie::GameGenieCode> gameGenieCodes{};
		BYTE szInesHeader[16]{};
		std::unordered_map<WORD, GameGenie::GameGenieCode*> activeCodeMap{};

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
		void inline BuildCodeMap()
		{
			activeCodeMap.clear();

			for (auto& code : gameGenieCodes)
			{
				if (!code.isActive)
					continue;

				activeCodeMap[code.decoded.addr] = (&code);
			}
		}

		// if the header doesn't begin with 'N', the header was loaded - header always starts with "NES"
		const bool inline IsHeaderNull() const { return (szInesHeader[0] != 'N' || szInesHeader[1] != 'E' || szInesHeader[2] != 'S' || szInesHeader[3] != 0x1A); }
	};

	static std::vector<BYTE> HexStrToBytes(const std::string& hexString);
	static std::vector<BYTE> LoadFile(const std::string& filePath);
	static std::string GetFullFilePath(const std::string& relativePath);
	static Loader::GameInfo FindROM(const std::vector<BYTE>* romRaw, const std::string& dbPath);
};
