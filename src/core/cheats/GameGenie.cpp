#include "GameGenie.h"


GameGenie::GameGenie()
{

}


GameGenie::DecodedCode GameGenie::Decode(const std::string& code)
{
    const size_t numBits = code.length() * 4;

    std::vector<int> positionMap{};
    if (numBits == 24)
        positionMap = POSITION_MAP_6;
    else if (numBits == 32)
        positionMap = POSITION_MAP_8;
    else
        return {}; // TODO: this should throw an error
        //throw std::invalid_argument("String length was not 6 or 8!");
    
    GameGenie::DecodedCode decodedCode{};


    // e.g. code was SXIOPO, so encodedBits is 1101 1010 0101 1001 0001 1001 (leftmost position is 23)
    std::vector<bool> encodedBits = StringCodeToBits(code);
    std::vector<bool> decodedBits(numBits, 0);
    for (int i = 0; i < numBits; ++i)
    {
        if (positionMap.at(i) == -1)
        {
            decodedBits[(numBits - 1) - i] = 0;
            continue;
        }

        decodedBits[(numBits - 1) - positionMap.at(i)] = encodedBits[(numBits - 1) - i];
    }
    

    // now decodedBits is 1010 1101 0001 0001 1101 1001 (leftmost position is 23)

    // first 8 bits (refers to the left hand side (1010 1101)), evaluates to the value
    std::vector<bool> bitsValue = std::vector<bool>(decodedBits.end() - 8, decodedBits.end()); // 1010 1101

    // get next 15 bits, which evaluates to the address
    std::vector<bool> bitsAddress = std::vector<bool>(decodedBits.end() - 24, decodedBits.end() - 9); // 001 0001 1101 1001

    // if the string was of 8 length, get the compare address (last 8 bits)
    std::vector<bool> bitsCompare{};
    if (decodedBits.size() >= 32)
        bitsCompare = std::vector<bool>(decodedBits.end() - 32, decodedBits.end() - 24);
        

    auto BitsToInt = [](const std::vector<bool>& bits) -> int { // processes bits from LEFT TO RIGHT
        int val = 0;
        for (size_t i = 0; i < bits.size(); ++i)
        {
            val |= bits[i] << i; // process left to right
        }
        return val;
    };

    decodedCode.val = BitsToInt(bitsValue); // extract high 8 bits
    decodedCode.addr = BitsToInt(bitsAddress); // ignore 9th bit and extract next 15 bits
    if (decodedBits.size() >= 32)
        decodedCode.compare = BitsToInt(bitsCompare); // extract low 8 bits

    return decodedCode;
}


// Maps each character to its equivalent hex value in the hex table, and returns a vector<bool> as a bitset.
// e.g. code is SXIOPO, return value would be:
// 1101 1010 0101 1001 0001 1001
// where bits[23] is the left side, and bits[0] is the right side (bits[22] == 1, bits[1] == 0)
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