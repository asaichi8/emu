#pragma once

#include <cstring>
#include <vector>
#include <sstream>
#include <fstream>
#include <cstdint>
#include <iostream>
#include "../../include/typedefs.h"


class Loader
{

public:
	Loader();

	std::vector<BYTE> HexStrToBytes(const std::string& hexString);
	std::vector<BYTE> LoadFile(const std::string& filePath);
	std::string GetFullFilePath(const char* relativePath);
};
