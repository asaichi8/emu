#pragma once

#include <cstring>
#include <vector>
#include <sstream>
#include <fstream>
#include <cstdint>
#include <iostream>
#include <filesystem>
#include "../../include/typedefs.h"


class Loader
{

public:
	Loader();

	static std::vector<BYTE> HexStrToBytes(const std::string& hexString);
	static std::vector<BYTE> LoadFile(const std::string& filePath);
	static std::string GetFullFilePath(const std::string& relativePath);
};
