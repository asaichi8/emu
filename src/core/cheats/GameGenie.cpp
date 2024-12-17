#include "GameGenie.h"


GameGenie::GameGenie()
{

}


std::vector<bool> GameGenie::StringCodeToBits(const std::string& code)
{
    if (code.empty())
        throw std::invalid_argument("String length must be greater than 0!");

    const size_t numBits = code.length() * 4;
    std::vector<bool> bits(numBits, 0);

    for (int i = 0; i < code.length(); ++i)
    {
        char c = code.at(i);

        auto it = HexTable.find(c);
        if (it == HexTable.end())
            throw std::invalid_argument("Not a valid Game Genie code!");
        
        size_t shiftVal = numBits - ((i + 1) * 4); // hex values go from left to right rather than right to left
        
        for (int j = 3; j >= 0; --j)
        {
            bits.at(shiftVal + j) = (it->second >> j) & 1;
        }
    }

    return bits;
}

GameGenie::DecodedCode GameGenie::Decode(const std::string& code)
{
    const size_t numBits = code.length() * 4;
    if (numBits != 24 && numBits != 32)
        throw std::invalid_argument("String length was not 6 or 8!");
    
    GameGenie::DecodedCode decodedCode{};

    std::vector<bool> encodedBits = StringCodeToBits(code);


    // std::bitset<numBits> encodedBits = StringCodeToBits(code);
    // std::bitset<numBits> decodedBits{};
    // for (int i = 0; i < numBits; ++i)
    // {
    //     if (POSITION_MAP_6.at(i) == -1)
    //     {
    //         decodedBits[(numBits - 1) - i] = 0;
    //         continue;
    //     }

    //     decodedBits[(numBits - 1) - POSITION_MAP_6.at(i)] = encodedBits[(numBits - 1) - i];
    // }

    // decodedCode.val = (decodedBits.to_ulong() >> 16) & 0xFF; // extract high 8 bits
    // decodedCode.addr = decodedBits.to_ulong() & 0x7FFF; // extract low 15 bits

    // delete encodedBits;
    return decodedCode;
}