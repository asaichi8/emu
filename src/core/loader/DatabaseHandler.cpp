#include "DatabaseHandler.h"


std::string DatabaseHandler::CalcMD5(const std::vector<BYTE>& file)
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


std::string DatabaseHandler::HashToHexStr(const BYTE hash[], size_t len)
{
	std::stringstream ss;

	for (int i = 0; i < len; ++i)
	{
		ss << std::hex << std::setw(2) << std::setfill('0') << (int)(hash[i]);
	}

	return ss.str();
}

nlohmann::json DatabaseHandler::GetJSONDatabase(const std::string &dbPath)
{
    nlohmann::json j;

    std::ifstream db(dbPath);
    if (!db.is_open())
    {
        std::cerr << "Could not open JSON database!" << std::endl;
        return {};
    }

    try
    {
        db >> j;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Failed to parse JSON database: " << e.what() << std::endl;
        db.close();
        return {};
    }

    db.close();
    return j;
}

bool DatabaseHandler::InsertInfo(const Loader::GameInfo& info, const std::string &md5, const std::string &md5headerless, const std::string& dbPath, bool shouldReplace)
{
    if (md5.empty() && md5headerless.empty())
        return false;

    using json = nlohmann::json;

    json j = GetJSONDatabase(dbPath);

    // find roms array & create one if it doesnt exist
	if (!j.contains("roms") || !j["roms"].is_array())
	{
        j["roms"] = json::array();
	}

    json* curRom = nullptr;

    // loop through every rom & attempt to find our rom based on md5 hash.
    for (auto& rom : j["roms"])
    {
        bool foundRom = false;
		
		// search for both md5 & md5-headerless in each header
		if (rom.contains("md5") && (rom["md5"] == md5 || (!md5headerless.empty() && rom["md5"] == md5headerless)))
			foundRom = true;
		else if (rom.contains("md5-headerless") && (rom["md5-headerless"] == md5 || (!md5headerless.empty() && rom["md5-headerless"] == md5headerless)))
			foundRom = true;

		if (foundRom)
        {
            curRom = &rom;
            break;
        }
    }

    // if we didn't find the rom, we'll just create a json entry, otherwise we'll modify it
    if (!curRom)
        curRom = &j["roms"].emplace_back(json::object());

    auto ShouldUpdate = [&curRom, shouldReplace](const std::string& field) {
        return shouldReplace || (!shouldReplace && !curRom->contains(field));
    };

    if (!md5.empty() && ShouldUpdate("md5"))
        (*curRom)["md5"] = md5;

    if (!info.name.empty() && ShouldUpdate("name"))
        (*curRom)["name"] = info.name;

    if (!md5headerless.empty() && ShouldUpdate("md5-headerless"))
        (*curRom)["md5-headerless"] = md5headerless;

    if (!info.gameGenieCodes.empty())
    {
        json genieArray;

        if (shouldReplace) // if we should replace it, just create a new array
            genieArray = json::array();
        else
            if (curRom->contains("game_genie_codes") && (*curRom)["game_genie_codes"].is_array())
                genieArray = (*curRom)["game_genie_codes"]; // shouldn't replace and we found our array, so append to it
            else
                genieArray = json::array(); // couldnt find our array so just create a new one

        // for each code in info...
        for (const auto& codeObj : info.gameGenieCodes)
        {
            std::string code = GameGenie::ConcatenateCodes(codeObj.code, " + ");

            json jCode;
            jCode["code"] = code;
            jCode["description"] = codeObj.description;
            jCode["is_active"] = codeObj.isActive;

            genieArray.push_back(jCode);
        }

        (*curRom)["game_genie_codes"] = genieArray;
    }

    // attempt to write our new changes to the file (we need to replace the entire file because json)
    try
    {
        std::ofstream db(dbPath, std::ios::trunc);
        if (!db.is_open())
        {
            std::cerr << "Couldn't truncate database!" << std::endl;
            return false;
        }

        db << j.dump(4);

        if (!db)
        {
            std::cerr << "Failed to write all data to databasee!" << std::endl;
            return false;
        }

        db.close();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Failed to write to database: " << e.what() << std::endl;
        return false;
    }

    return true;
}

/// @brief Gets the MD5 and headerless MD5 of a given ROM.
/// @param romRaw A vector containing the entire ROM.
/// @return A pair of strings - the first being the MD5 value of the ROM (which is empty on failure), and the second
///			being the headerless MD5 value of the ROM (which is empty if inapplicable).
std::pair<std::string, std::string> DatabaseHandler::GetMD5(const std::vector<BYTE>* romRaw)
{
	if (!romRaw || romRaw->empty() || romRaw->size() < NES_HEADER_SIZE + 1)
	{
		std::cerr << "Invalid usage of GetMD5()" << std::endl;
		return {{}, {}};
	}

	// we must be able to calculate an md5 for the raw file, otherwise give up
    std::string md5 = CalcMD5(*romRaw);
	// std::cout << "md5: " << md5 << std::endl;
	if (md5.empty())
		return {{}, {}};

	// we can optionally calculate a hash for the headerless version of the file, just in case the main hash fails. ROM dumpers
	// may choose different header values, so this increases the success rate of finding the ROM greatly.
	std::string md5headerless{};
	if (ROM::CheckINES(romRaw))
	{
		std::vector<BYTE> romCopy = *romRaw;
		romCopy.erase(romCopy.begin(), romCopy.begin() + NES_HEADER_SIZE);

		md5headerless = CalcMD5(romCopy);
		// std::cout << "md5-headerless: " << md5headerless << std::endl;
	}

	return {md5, md5headerless};
}

// TODO: this should probably just take md5s, add it to the wrappers maybe
Loader::GameInfo DatabaseHandler::FindROMGameInfo(const std::vector<BYTE>* romRaw, const nlohmann::json& j)
{
    if (!romRaw || romRaw->empty())
    {
        std::cerr << "Error with romRaw!" << std::endl;
        return {};
    }

	using json = nlohmann::json;
	Loader::GameInfo info{};

	auto md5pair = GetMD5(romRaw);
	std::string md5 = md5pair.first;
	std::string md5headerless = md5pair.second;

	if (!j.contains("roms") || !j["roms"].is_array())
	{
		std::cerr << "Invalid JSON format!" << std::endl;
		return {};
	}

	for (const auto& rom : j["roms"])
	{
		bool foundRom = false;
		
		// search for both md5 & md5-headerless in each header, just in case
		// search for headered md5
		if (rom.contains("md5") && (rom["md5"] == md5 || (!md5headerless.empty() && rom["md5"] == md5headerless)))
			foundRom = true;
		else if (rom.contains("md5-headerless") && (rom["md5-headerless"] == md5 || (!md5headerless.empty() && rom["md5-headerless"] == md5headerless)))
		{
			// search for headerless md5
			std::cout << "found headerless" << std::endl;
			foundRom = true;
		}

		if (!foundRom)
			continue;

		
		// attempt to extract name
		if (rom.contains("name"))
			info.name = rom["name"].get<std::string>();
		else
			info.name = "Unknown";
		
		// attempt to extract game genie codes, if we have any
		if (rom.contains("game_genie_codes") && rom["game_genie_codes"].is_array())
		{
			for (const auto& codeStruct : rom["game_genie_codes"])
			{
				// we must have a code and its description
				if (!codeStruct.contains("code") || !codeStruct.contains("description"))
					continue;
				
				std::string strCode = codeStruct["code"];
				std::string strDescription = codeStruct["description"];

				// may contain an is_active field, but if not, set it to false as default
				bool isActive = false;
				if (codeStruct.contains("is_active"))
					isActive = codeStruct["is_active"].get<bool>();

				GameGenie::GameGenieCode finalCode(GameGenie::ExtractCodes(strCode, '+'), strDescription, isActive);
				info.gameGenieCodes.emplace_back(finalCode);
			}

			// info.BuildCodeMap();
		}

		// attempt to extract iNES header, if it exists
		if (rom.contains("ines-header"))
			info.SetHeaderFromHexStr(rom["ines-header"].get<std::string>());

		return info;
	}
	
	std::cerr << "couldn't find hash" << std::endl;
    return {};
}

Loader::GameInfo DatabaseHandler::FindROMGameInfoW(const std::vector<BYTE> *romRaw, const nlohmann::json &j)
{
    return FindROMGameInfo(romRaw, j);
}
Loader::GameInfo DatabaseHandler::FindROMGameInfoW(const std::vector<BYTE> *romRaw, const std::string &dbPath)
{
	nlohmann::json j = GetJSONDatabase(dbPath);

    return FindROMGameInfo(romRaw, j);
}

bool DatabaseHandler::InsertInfoW(const Loader::GameInfo& info, const std::vector<BYTE> *romRaw, const std::string& dbPath, bool shouldReplace)
{
	auto md5pair = GetMD5(romRaw);
	std::string md5 = md5pair.first;
	std::string md5headerless = md5pair.second;

    return InsertInfo(info, md5, md5headerless, dbPath, shouldReplace);
}
bool DatabaseHandler::InsertInfoW(const Loader::GameInfo& info, const std::string &md5, const std::string &md5headerless, const std::string& dbPath, bool shouldReplace)
{
    return InsertInfo(info, md5, md5headerless, dbPath, shouldReplace);
}

