#pragma once

#include <vector>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <openssl/evp.h>
#include "../../include/typedefs.h"
#include "Loader.h"


class DatabaseHandler
{
	static const size_t NES_HEADER_SIZE = 16;
    
	static std::string CalcMD5(const std::vector<BYTE>& file);
	static std::string HashToHexStr(const BYTE hash[], size_t len);

public:
	static std::pair<std::string, std::string> GetMD5(const std::vector<BYTE>* romRaw);
	static Loader::GameInfo FindROMGameInfo(const std::vector<BYTE>* romRaw, const std::string& dbPath);
};