#pragma once

#include <unordered_map>
#include <optional>
#include <stdexcept>
#include <vector>
#include <algorithm>
#include <sstream>
#include <iostream>
#include "typedefs.h"


// https://github.com/jdratlif/ggencoder-java/blob/master/docs/nes.txt
class GameGenie
{
    // string:   SXIOPO
    // from link above:
    // Code:     110110100101100100011001
    // Position: 1678H234-IJKLABCDMNO5EFG

    // so, e.g. place the leftmost bit at 1 in the rearranged code, the second leftmost bit at position 6 in the
    // rearranged code etc
    // Original:   1101 1010 0101 1001 0001 1001
    // Rearranged: 1010 1101 0001 0001 1101 1001
    // Represents: VVVV VVVV -AAA AAAA AAAA AAAA
    static const inline std::vector<int> POSITION_MAP_6 = { // where each bit should go in a 6-length game genie code
        0, 5, 6, 7, 16, 1, 2, 3, -1, 17, 18, 19, 20, 9, 10, 11, 12,
        21, 22, 23, 4, 13, 14, 15
    };
    static const inline std::vector<int> POSITION_MAP_8 = { // 8-length game genie code
        0, 5, 6, 7, 16, 1, 2, 3, -1, 17, 18, 19, 20, 9, 10, 11, 12,
        21, 22, 23, 28, 13, 14, 15, 24, 29, 30, 31, 4, 25, 26, 27
    };
    // in the game genie, each letter evaluates to an appropriate binary value
    static const inline std::unordered_map<char, BYTE> HexTable = {
        {'A', 0x0}, {'P', 0x1}, {'Z', 0x2}, {'L', 0x3}, {'G', 0x4}, {'I', 0x5}, {'T', 0x6}, {'Y', 0x7},
        {'E', 0x8}, {'O', 0x9}, {'X', 0xA}, {'U', 0xB}, {'K', 0xC}, {'S', 0xD}, {'V', 0xE}, {'N', 0xF}
    };

    static std::vector<bool> StringCodeToBits(const std::string& code);
    static std::string BitCodeToString(const std::vector<bool>& code);

public:
    GameGenie();

    struct DecodedCode
    {
        WORD addr{};
        BYTE val{};
        std::optional<BYTE> compare = std::nullopt;
    };
    
	struct GameGenieCode
	{
		std::vector<std::string> code{};
		std::string description{};
		bool isActive = false;
		std::vector<GameGenie::DecodedCode> decoded{};

		GameGenieCode(const std::vector<std::string>& _code, const std::string& _description, bool _isActive)
			: code(_code), description(_description), isActive(_isActive)
		{
            for (const auto& c : code)
            {
                decoded.push_back(GameGenie::Decode(c));
            }
		}
	};

    static DecodedCode Decode(const std::string& code);
    static std::string Encode(const DecodedCode& decodedCode);
    static std::vector<std::string> ExtractCodes(const std::string& codes, char delimiter);
};