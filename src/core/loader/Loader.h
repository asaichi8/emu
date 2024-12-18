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


class Loader
{
	static std::string HashToHexStr(const BYTE hash[], size_t len);
	static std::string CalcMD5(const std::vector<BYTE>& file);

public:
	Loader();

	struct GameGenieCode
	{
		std::string code{};
		std::string description{};
		bool isActive = false;

		GameGenieCode(const std::string& _code, const std::string& _description, bool _isActive)
			: code(_code), description(_description), isActive(_isActive) {}
	};

	struct GameInfo
	{
		std::string name{};
		std::string md5{};
		std::vector<GameGenieCode> gameGenieCodes{};
	};

	static std::vector<BYTE> HexStrToBytes(const std::string& hexString);
	static std::vector<BYTE> LoadFile(const std::string& filePath);
	static std::string GetFullFilePath(const std::string& relativePath);
	static GameInfo FindROM(const std::vector<BYTE>* romRaw, const std::string& dbPath);
};
