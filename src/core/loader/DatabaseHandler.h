#pragma once

#include <vector>
#include "pch.h"
#include <iomanip>
#include <sstream>
#include <fstream>
#include <openssl/evp.h>
#include "../../include/typedefs.h"
#include "ROM.h"


class DatabaseHandler
{
	static const size_t NES_HEADER_SIZE = 16;

	static std::string CalcMD5(const std::vector<BYTE>& file);
	static std::string HashToHexStr(const BYTE hash[], size_t len);
    static nlohmann::json GetJSONDatabase(const std::string& dbPath);
    static bool InsertInfo(const Loader::GameInfo& info, const std::string& md5, const std::string& md5headerless, const std::string& dbPath, bool shouldReplace = false);
	static Loader::GameInfo FindROMGameInfo(const std::vector<BYTE>* romRaw, const nlohmann::json& j);

public:
	static std::pair<std::string, std::string> GetMD5(const std::vector<BYTE>* romRaw);
    static bool InsertInfoW(const Loader::GameInfo& info, const std::vector<BYTE>* romRaw, const std::string& dbPath, bool shouldReplace = false);
    static bool InsertInfoW(const Loader::GameInfo& info, const std::string& md5, const std::string& md5headerless, const std::string& dbPath, bool shouldReplace = false);
    static Loader::GameInfo FindROMGameInfoW(const std::vector<BYTE>* romRaw, const std::string& dbPath);
    static Loader::GameInfo FindROMGameInfoW(const std::vector<BYTE>* romRaw, const nlohmann::json& j);
};