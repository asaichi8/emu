#include "GameGenie.h"


GameGenie::GameGenie()
{

}


std::bitset<24> GameGenie::StringCodeToBits(const std::string& code)
{
    if (code.length() != 6)
        throw std::invalid_argument("String length was not 6!");

    std::bitset<24> bits{};

    for (int i = 0; i < code.length(); ++i)
    {
        char c = code.at(i);

        auto it = HexTable.find(c);
        if (it == HexTable.end())
            throw std::invalid_argument("Not a valid Game Genie code!");
        
        size_t shiftVal = 20 - (i * 4); // hex values go from left to right rather than right to left
        bits |= (it->second.to_ulong() << shiftVal);
    }

    return bits;
}

GameGenie::DecodedCode GameGenie::Decode(const std::string& code)
{
    if (code.length() != 6)
        throw std::invalid_argument("String length was not 6!");
    
    GameGenie::DecodedCode decodedCode{};

    std::bitset<24> encodedBits = StringCodeToBits(code);
    std::bitset<24> decodedBits{};
    for (int i = 0; i < 24; ++i)
    {
        if (POSITION_MAP.at(i) == -1)
        {
            decodedBits[23 - i] = 0;
            continue;
        }

        decodedBits[23 - POSITION_MAP.at(i)] = encodedBits[23 - i];
    }

    decodedCode.val = (decodedBits.to_ulong() >> 16) & 0xFF; // extract high 8 bits
    decodedCode.addr = decodedBits.to_ulong() & 0x7FFF; // extract low 15 bits

    return decodedCode;
}